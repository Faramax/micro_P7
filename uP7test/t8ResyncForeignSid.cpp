////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                     /
// This library is free software; you can redistribute it and/or modify it under the terms of the provided License.    /
//                                                                                                                     /
// This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even  the  implied/
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more/
// details.                                                                                                            /
// You should have received a copy of the the License along with this library.                                         /
//                                                                                                                     /
// 2012-2024 (c) Baical                                                                                                /
//                                                                                                                     /
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Regression test for the uP7 proxy resynchronisation livelock.
// The stream gets [foreign header][valid packet][foreign header] in one piece: the first foreign header switches the
// proxy to resync, the resync run starts at the valid packet and then reaches a header of another session with a
// valid size. Before the fix CProxyCpu::SynchronizeSession() never left its inner loop in this case.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "common.h"
#include <atomic>
#include <vector>

#define T8_MESSAGES_PER_CYCLE  100
#define T8_DELIVERY_TIMEOUT_MS 3000

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CT8P7Sink: public CClTextSink
{
   size_t     m_szCount;
   std::mutex m_cLock;
public:
   CT8P7Sink()
   {
      m_szCount = 0;
   }

   virtual ~CT8P7Sink()
   {
   }

   virtual eClient_Status Initialize(tXCHAR **i_pArgs, tINT32 i_iCount)
   {
      UNUSED_ARG(i_pArgs);
      UNUSED_ARG(i_iCount);
      return ECLIENT_STATUS_OK;
   }

   virtual eClient_Status Log(const CClTextSink::sLog &i_rRawLog,
                              const tXCHAR            *i_pFmtLog,
                              size_t                   i_szFmtLog
                             )
   {
      UNUSED_ARG(i_pFmtLog);
      UNUSED_ARG(i_szFmtLog);
      m_cLock.lock();
      if (    (i_rRawLog.pModuleName)
           && (0 == PStrCmp(i_rRawLog.pModuleName, TM("t8")))
         )
      {
         m_szCount++;
      }
      else
      {
         PRINTF(TM("%s\n"), i_rRawLog.pMessage);
      }
      m_cLock.unlock();

      return ECLIENT_STATUS_OK;
   }

   virtual eClient_Status TryRoll()
   {
      return ECLIENT_STATUS_OK;
   }

   virtual eClient_Status DumpBuffers()
   {
      return ECLIENT_STATUS_OK;
   }

   size_t GetCount()
   {
      m_cLock.lock();
      size_t l_szRet = m_szCount;
      m_cLock.unlock();

      return l_szRet;
   }

   void ClearCount()
   {
      m_cLock.lock();
      m_szCount = 0;
      m_cLock.unlock();
   }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////uP7 callbacks/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//size of the foreign headers to inject around the next packet, 0 - no injection
static std::atomic<uint16_t> g_wT8InjectSize(0);

static uint64_t t8getTimerFrequency(void *i_pCtx)  { UNUSED_ARG(i_pCtx); return GetPerformanceFrequency(); }
static uint64_t t8getTimerValue(void *i_pCtx)      { UNUSED_ARG(i_pCtx); return GetPerformanceCounter();   }
static uint32_t t8getCurrentThreadId(void *i_pCtx) { UNUSED_ARG(i_pCtx); return CProc::Get_Thread_Id();    }

static void t8appendForeignHeader(std::vector<uint8_t> &io_rData, uint16_t i_wSize)
{
   stuP7baseHdr l_stHdr;
   l_stHdr.uSessionId = ~g_uSessionId;
   l_stHdr.wSize      = i_wSize;
   l_stHdr.bType      = uP7packetTrace;
   l_stHdr.bExt1Crc7  = g_bCrc7;

   const uint8_t *l_pHdr = reinterpret_cast<const uint8_t *>(&l_stHdr);
   io_rData.insert(io_rData.end(), l_pHdr, l_pHdr + sizeof(l_stHdr));
}

static bool t8sendPacket(void                         *i_pCtx,
                         enum euP7packetRank           i_eRank,
                         const struct stP7packetChunk *i_pChunks,
                         size_t                        i_szChunks,
                         size_t                        i_szData)
{
   UNUSED_ARG(i_eRank);
   IuP7Fifo *l_pFifo = static_cast<IuP7Fifo*>(i_pCtx);

   if (l_pFifo->GetFreeBytes() < (i_szData + 2 * sizeof(stuP7baseHdr)))
   {
      return false;
   }

   uint16_t             l_wInject = g_wT8InjectSize.exchange(0);
   std::vector<uint8_t> l_cData;
   l_cData.reserve(i_szData + 2 * sizeof(stuP7baseHdr));

   if (l_wInject)
   {
      t8appendForeignHeader(l_cData, l_wInject);
   }

   for (size_t l_szI = 0; l_szI < i_szChunks; l_szI++)
   {
      const uint8_t *l_pChunk = static_cast<const uint8_t *>(i_pChunks[l_szI].pData);
      l_cData.insert(l_cData.end(), l_pChunk, l_pChunk + i_pChunks[l_szI].szData);
   }

   if (l_wInject)
   {
      t8appendForeignHeader(l_cData, l_wInject);
   }

   //single write: the whole sequence lands in one proxy buffer and is processed by one SynchronizeSession() call
   return l_pFifo->Write(l_cData.data(), l_cData.size());
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool t8waitCount(CT8P7Sink &i_rSink, size_t i_szExpected, uint32_t i_uTimeoutMs)
{
   uint32_t l_uStart = GetTickCount();
   while (i_rSink.GetCount() < i_szExpected)
   {
      if (CTicks::Difference(GetTickCount(), l_uStart) >= i_uTimeoutMs)
      {
         return false;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
   }

   return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool t8ResyncForeignSid(const tXCHAR *i_pSessionFolder)
{
   CT8P7Sink   l_cP7Sink;
   uint64_t    l_qwAdd = (uint64_t)(&l_cP7Sink);
   tXCHAR      l_pArgs[4096];
   bool        l_bReturn = true;
   bool        l_bStuck  = false;

   PSPrint(l_pArgs, LENGTH(l_pArgs), TM("/P7.Verb=4 /P7.Sink=ExternalSinc /P7.ExtAddr=%llX /P7.Pool=16384"), l_qwAdd);

   IuP7proxy *l_iProxy = uP7createProxy(l_pArgs, i_pSessionFolder);
   IuP7Fifo  *l_iFifo  = NULL;

   if (!l_iProxy->RegisterCpu(0, false, GetPerformanceFrequency(), TM("T8"), 128*1024, true, l_iFifo))
   {
      return false;
   }

   struct stuP7config l_stConfig;

   l_stConfig.bSessionIdCrc7        = g_bCrc7;
   l_stConfig.uSessionId            = g_uSessionId;
   l_stConfig.pCtxTimer             = NULL;
   l_stConfig.cbTimerFrequency      = t8getTimerFrequency;
   l_stConfig.cbTimerValue          = t8getTimerValue;
   l_stConfig.pCtxLock              = NULL;
   l_stConfig.cbLock                = NULL;
   l_stConfig.cbUnLock              = NULL;
   l_stConfig.pCtxPacket            = l_iFifo;
   l_stConfig.cbSendPacket          = t8sendPacket;
   l_stConfig.pCtxThread            = 0;
   l_stConfig.cbGetCurrentThreadId  = t8getCurrentThreadId;
   l_stConfig.eDefaultVerbosity     = euP7Level_Trace;
   l_stConfig.pModules              = g_pModules;
   l_stConfig.szModules             = g_szModules;
   l_stConfig.pTraces               = g_pTraces;
   l_stConfig.szTraces              = g_szTraces;
   l_stConfig.pTelemetry            = g_pTelemetry;
   l_stConfig.szTelemetry           = g_szTelemetry;
   l_stConfig.eAlignment            = euP7alignment4;

   if (!uP7initialize(&l_stConfig))
   {
      return false; //we don't care about memory leak! test failed application will be closed
   }

   huP7Module l_hModule = uP7_MODULE_INVALID_ID;
   if (!uP7TrcRegisterModule("t8", euP7Level_Trace, &l_hModule))
   {
      return false;
   }

   std::atomic<bool>   l_bExit(false);
   std::atomic<size_t> l_szMessages(0);

   //uP7 is used without locks, so all uP7 calls are made from this single thread
   std::thread *l_pThread = new std::thread([&]
   {
      stReadContext l_stReadCtx = {0};
      l_stReadCtx.pFifo = l_iFifo;

      while (!l_bExit)
      {
         uP7ProcessIncomingData(&l_stReadCtx, 0);

         size_t l_szCount = l_szMessages;
         for (size_t l_szI = 0; l_szI < l_szCount; l_szI++)
         {
            uP7ERR(51, l_hModule, "T8 test message #%05zu", l_szI);
         }
         if (l_szCount)
         {
            l_szMessages = 0;
         }

         std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
   });

   //warm-up: let the proxy recognize the session
   l_szMessages = T8_MESSAGES_PER_CYCLE;
   if (!t8waitCount(l_cP7Sink, T8_MESSAGES_PER_CYCLE, T8_DELIVERY_TIMEOUT_MS))
   {
      printf("ERROR: warm-up, received: %zu\n", l_cP7Sink.GetCount());
      l_bReturn = false;
   }

   const uint16_t l_pSizes[] = {1, 7, 8, 16, 64, 255, 512, 1024, 4095};

   for (size_t l_szI = 0; (l_bReturn) && (l_szI < LENGTH(l_pSizes)); l_szI++)
   {
      while (0 != l_szMessages)
      {
         std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }

      //wait for flushing ...
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
      l_cP7Sink.ClearCount();

      g_wT8InjectSize = l_pSizes[l_szI];
      l_szMessages    = T8_MESSAGES_PER_CYCLE;

      //the packet wrapped by the foreign headers is dropped together with the resync run
      if (!t8waitCount(l_cP7Sink, T8_MESSAGES_PER_CYCLE - 1, T8_DELIVERY_TIMEOUT_MS))
      {
         printf("ERROR: foreign header size %u, received: %zu of %d, proxy is stuck\n",
                (unsigned)l_pSizes[l_szI], l_cP7Sink.GetCount(), T8_MESSAGES_PER_CYCLE);
         l_bReturn = false;
         l_bStuck  = true;
         break;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(250));
      if (l_cP7Sink.GetCount() > T8_MESSAGES_PER_CYCLE)
      {
         printf("ERROR: foreign header size %u, received: %zu of %d\n",
                (unsigned)l_pSizes[l_szI], l_cP7Sink.GetCount(), T8_MESSAGES_PER_CYCLE);
         l_bReturn = false;
         break;
      }
   }

   l_bExit = true;
   l_pThread->join();
   delete l_pThread;
   l_pThread = NULL;

   uP7unInitialize();

   if (l_bStuck)
   {
      //the proxy thread never returns, releasing the proxy would hang the test
      return false;
   }

   if (l_iFifo)
   {
      l_iProxy->UnRegisterCpu(0);
      l_iFifo->Release();
   }

   if (l_iProxy)
   {
      l_iProxy->Release();
      l_iProxy = NULL;
   }

   return l_bReturn;
}
