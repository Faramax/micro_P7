////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                     /
// This library is free software; you can redistribute it and/or modify it under the terms of the  GNU  Lesser  General/
// Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your  option)/
// any later version.                                                                                                  /
// This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even  the  implied/
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more/
// details.                                                                                                            /
// You should have received a copy of the GNU Lesser General Public License along with this library.                   /
//                                                                                                                     /
// 2012-2021 (c) Baical                                                                                                /
//                                                                                                                     /
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "common.h"
#include <atomic>

#define T5_DURATION_MS 10000

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CT6P7Sink: public CClTextSink
{
    size_t      m_szCount;
    std::mutex  m_cLock;
public:
    CT6P7Sink()
    {
        m_szCount = 0;
    }

    virtual ~CT6P7Sink()
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
             && (0 == PStrCmp(i_rRawLog.pModuleName, TM("t6")))
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
static std::atomic<bool>     g_bCut(false);
static std::atomic<uint64_t> g_qwCutOffset(0);
static std::atomic<uint64_t> g_qwBytesCount(0);

uint64_t t6getTimerFrequency(void *i_pCtx)      { UNUSED_ARG(i_pCtx); return GetPerformanceFrequency(); }
uint64_t t6getTimerValue(void *i_pCtx)          { UNUSED_ARG(i_pCtx); return GetPerformanceCounter();   }
uint32_t t6getCurrentThreadId(void *i_pCtx)     { UNUSED_ARG(i_pCtx); return CProc::Get_Thread_Id();    }
bool     t6sendPacket(void                         *i_pCtx, 
                      enum euP7packetRank           i_eRank, 
                      const struct stP7packetChunk *i_pChunks,
                      size_t                        i_szChunks, 
                      size_t                        i_szData)
{
    UNUSED_ARG(i_eRank);
    IuP7Fifo *l_pFifo = static_cast<IuP7Fifo*>(i_pCtx);

    if (l_pFifo->GetFreeBytes() < i_szData)
    {
        return false;
    }

    while (i_szChunks)
    {
        g_qwBytesCount += i_pChunks->szData;

        if (    (g_bCut) 
             && (i_pChunks->szData > 1)
             && (g_qwBytesCount >= g_qwCutOffset)
           )
        {
            g_bCut = false;
            if (!l_pFifo->Write(((const uint8_t*)i_pChunks->pData) + 1, i_pChunks->szData - 1))
            {
                break;
            }
        }
        else if (!l_pFifo->Write(i_pChunks->pData, i_pChunks->szData))
        {
            break;
        }
        i_pChunks++;
        i_szChunks--;
    }

    return (0 == i_szChunks);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool t6Resync(const tXCHAR *i_pSessionFolder)
{
    CT6P7Sink   l_cP7Sink; 
    uint64_t    l_qwAdd = (uint64_t)(&l_cP7Sink);
    tXCHAR      l_pArgs[4096];
    bool        l_bReturn = true;

    PSPrint(l_pArgs, LENGTH(l_pArgs), TM("/P7.Verb=0 /P7.Sink=ExternalSinc /P7.ExtAddr=%llX /P7.Pool=16384"), l_qwAdd);
    //PSPrint(l_pArgs, LENGTH(l_pArgs), TM("/P7.Verb=0 /P7.Sink=Baical /P7.Pool=16384"), 0);

    IuP7proxy *l_iProxy = uP7createProxy(l_pArgs, i_pSessionFolder);
    IuP7Fifo  *l_iFifo  = NULL; 

    if (!l_iProxy->RegisterCpu(0, false, GetPerformanceFrequency(), TM("T6"), 128*1024, true, l_iFifo))
    {
        return false;
    }

    struct stuP7config l_stConfig; 

    l_stConfig.bSessionIdCrc7        = g_bCrc7;               /**< Session ID crc */ 
    l_stConfig.uSessionId            = g_uSessionId;          /**< Session ID*/  
    l_stConfig.pCtxTimer             = NULL;                  /**< Context for timer functions */
    l_stConfig.cbTimerFrequency      = t6getTimerFrequency;   /**< callback to get system high-resolution timer frequency */
    l_stConfig.cbTimerValue          = t6getTimerValue;       /**< callback to get system high-resolution timer value */
    l_stConfig.pCtxLock              = NULL;                  /**< Context for lock functions, set it to NULL if there is no RTOS*/
    l_stConfig.cbLock                = NULL;                  /**< callback to call OS lock function, set it to NULL if there is no RTOS */
    l_stConfig.cbUnLock              = NULL;                  /**< callback to call OS unlock function, set it to NULL if there is no RTOS */
    l_stConfig.pCtxPacket            = l_iFifo;               /**< Context for data sending */
    l_stConfig.cbSendPacket          = t6sendPacket;          /**< callback to send data packet to host */  
    l_stConfig.pCtxThread            = 0;                     /**< Context for cbGetCurrentThreadId function */
    l_stConfig.cbGetCurrentThreadId  = t6getCurrentThreadId;  /**< callback get current thread ID, set it to NULL if there is no RTOS*/  
    l_stConfig.eDefaultVerbosity     = euP7Level_Trace;       /**< Default verbosity*/  

    //Generated descriptors, located in uP7preprocessed.h
    l_stConfig.pModules              = g_pModules;            /**< Trace modules descriptors, generated by uP7 preprocessor*/
    l_stConfig.szModules             = g_szModules;           /**< Trace modules descriptors count*/  
    l_stConfig.pTraces               = g_pTraces;             /**< Trace descriptors, generated by uP7 preprocessor*/  
    l_stConfig.szTraces              = g_szTraces;            /**< Trace descriptors count*/  
    l_stConfig.pTelemetry            = g_pTelemetry;          /**< Telemetry descriptors, generated by uP7 preprocessor*/  
    l_stConfig.szTelemetry           = g_szTelemetry;         /**< Telemetry descriptors count*/  
    l_stConfig.eAlignment            = euP7alignment4;

    if (!uP7initialize(&l_stConfig))
    {
        return false; //we don't care about memory leak! test failed application will be closed
    }

    huP7Module l_hModule1 = uP7_MODULE_INVALID_ID;
    huP7TelId  l_hCounter1 = uP7_TELEMETRY_INVALID_ID;
    if (    (!uP7TelCreateCounter("t6 Counter", 0, 0, 1, 1, true, &l_hCounter1))
         || (!uP7TrcRegisterModule("t6", euP7Level_Trace, &l_hModule1))
       )
    {
        return false;
    }


    std::atomic<bool>   l_bExit(false);
    std::atomic<size_t> l_szMessages(0);

    std::thread *l_pThread = new std::thread([&]
    {
        stReadContext l_stReadCtx = {0};
        l_stReadCtx.pFifo = l_iFifo;

        const char *l_pText[10] = {
            "*",
            "*1",
            "*22",
            "*333",
            "*4444",
            "*55555",
            "*666666",
            "*7777777",
            "*88888888",
            "*999999999"
        };


        while (!l_bExit)
        {
            uP7ProcessIncomingData(&l_stReadCtx, 0);

            for (size_t l_szI = 0; l_szI < l_szMessages; l_szI++)
            {
                uP7ERR(55, l_hModule1, "T6 test message #%05zu {%s}", l_szI, l_pText[l_szI % 10]);

                if ((l_szI + 1) == l_szMessages)
                {
                    l_szMessages = 0;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
    });

    size_t l_szCount = 1000;
    size_t l_szPrev  = 0;
    for (int iI = 1; iI < 10; iI++)
    {
        g_bCut = false;
        l_cP7Sink.ClearCount();

        l_szPrev = l_szCount * iI / 10;
        l_szMessages = l_szPrev;

        while ( 0!= l_szMessages)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        g_bCut         = true;
        g_qwCutOffset  = iI * 32;
        g_qwBytesCount = 0;

        l_szMessages = l_szCount - l_szPrev;

        while ( 0!= l_szMessages)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    

        if (l_cP7Sink.GetCount() != l_szCount - 1)
        {
            printf("ERROR: iteration %i, received: %zu\n", iI, l_cP7Sink.GetCount());
            l_bReturn = false;
            break;
        }
    }


    l_bExit = true;
    l_pThread->join();
    delete l_pThread;
    l_pThread = NULL;

    uP7unInitialize();

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
