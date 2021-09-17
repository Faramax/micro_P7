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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CT4P7Sink: public CClTextSink
{
    size_t      m_szErrors;
    uint64_t    m_qwCount;
    //int         m_pCount[256];
    std::mutex *m_pLock;
public:
    CT4P7Sink(std::mutex *i_pLock)
    {
        m_pLock    = i_pLock;
        m_qwCount  = 0;
        m_szErrors = 0;
        //memset(m_pCount, 0, sizeof(m_pCount));
    }

    virtual ~CT4P7Sink()
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
        m_pLock->lock();
        if (0 != PStrICmp(TM("MicroP7"), i_rRawLog.pChannel))
        {
            //int l_iId = 0;
            //PStrScan(i_rRawLog.pChannel, TM("%d"), &l_iId);
            //m_pCount[l_iId] ++;
            m_qwCount ++;
        }
        else 
        {
            if (i_rRawLog.eLevel > EP7TRACE_LEVEL_ERROR)
            {
                m_szErrors ++;
            }
        }
        m_pLock->unlock();

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

    size_t GetErrorsCount()
    {
        m_pLock->lock();
        size_t l_szReturn = m_szErrors;
        m_pLock->unlock();
        return l_szReturn;
    }

    uint64_t GetCount()
    {
        m_pLock->lock();
        uint64_t l_qwReturn = m_qwCount;
        m_pLock->unlock();
        return l_qwReturn;
    }
};


struct stCtx
{
    std::map<std::thread::id, IuP7Fifo*> cMap;
    std::mutex cLock;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////uP7 callbacks/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint64_t t4getTimerFrequency(void *i_pCtx)      { UNUSED_ARG(i_pCtx); return GetPerformanceFrequency(); }
uint64_t t4getTimerValue(void *i_pCtx)          { UNUSED_ARG(i_pCtx); return GetPerformanceCounter();   }
uint32_t t4getCurrentThreadId(void *i_pCtx)     { UNUSED_ARG(i_pCtx); return CProc::Get_Thread_Id();    }
bool     t4sendPacket(void *i_pCtx, enum euP7packetRank i_eRank, const struct stP7packetChunk *i_pChunks,
                      size_t  i_szChunks, size_t i_szData)
{
    UNUSED_ARG(i_eRank);
    stCtx *pContext = static_cast<stCtx*>(i_pCtx);

    pContext->cLock.lock();
    auto l_pIt = pContext->cMap.find(std::this_thread::get_id());
    pContext->cLock.unlock();



    if (l_pIt != pContext->cMap.end())
    {
        IuP7Fifo  *l_pFifo = l_pIt->second;

        if (l_pFifo->GetFreeBytes() < i_szData)
        {
            return false;
        }

        while (i_szChunks)
        {
            if (!l_pFifo->Write(i_pChunks->pData, i_pChunks->szData))
            {
                break;
            }
            i_pChunks++;
            i_szChunks--;
        }
    }
    else
    {
        printf("panic!");
    }

    return (0 == i_szChunks);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool t4MultiCore(uint8_t i_bCount, const tXCHAR *i_pSessionFolder)
{
    const int l_iTraces = 1000;
    stCtx     l_cContext;
    CT4P7Sink l_cP7Sink(&l_cContext.cLock); 
    uint64_t  l_qwAdd = (uint64_t)(&l_cP7Sink);
    tXCHAR    l_pArgs[4096];

    PSPrint(l_pArgs, LENGTH(l_pArgs), TM("/P7.Verb=0 /P7.Sink=ExternalSinc /P7.ExtAddr=%llX /P7.Eto=0 /P7.Pool=16384"), l_qwAdd);

    IuP7proxy *l_iProxy = uP7createProxy(l_pArgs, i_pSessionFolder);

    struct stuP7config l_stConfig; 

    l_stConfig.bSessionIdCrc7        = g_bCrc7;               /**< Session ID crc */ 
    l_stConfig.uSessionId            = g_uSessionId;          /**< Session ID*/  
    l_stConfig.pCtxTimer             = NULL;                  /**< Context for timer functions */
    l_stConfig.cbTimerFrequency      = t4getTimerFrequency;   /**< callback to get system high-resolution timer frequency */
    l_stConfig.cbTimerValue          = t4getTimerValue;       /**< callback to get system high-resolution timer value */
    l_stConfig.pCtxLock              = NULL;                  /**< Context for lock functions, set it to NULL if there is no RTOS*/
    l_stConfig.cbLock                = NULL;                  /**< callback to call OS lock function, set it to NULL if there is no RTOS */
    l_stConfig.cbUnLock              = NULL;                  /**< callback to call OS unlock function, set it to NULL if there is no RTOS */
    l_stConfig.pCtxPacket            = &l_cContext;           /**< Context for data sending */
    l_stConfig.cbSendPacket          = t4sendPacket;          /**< callback to send data packet to host */  
    l_stConfig.pCtxThread            = 0;                     /**< Context for cbGetCurrentThreadId function */
    l_stConfig.cbGetCurrentThreadId  = t4getCurrentThreadId;  /**< callback get current thread ID, set it to NULL if there is no RTOS*/  
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
    if (    (!uP7TelCreateCounter("t4 Counter", 0, 0, 1, 1, true, &l_hCounter1))
         || (!uP7TrcRegisterModule("t4", euP7Level_Trace, &l_hModule1))
       )
    {
        return false;
    }

    std::vector<std::thread*> l_cThreads;
    std::vector<IuP7Fifo*>    l_cFifos;

    for (uint8_t l_bI = 0; l_bI < i_bCount; l_bI++)
    {
        IuP7Fifo *l_pFifo = NULL;
        tXCHAR l_pChannel[16];
        PSPrint(l_pChannel, LENGTH(l_pChannel), TM("%d"), l_bI);

        l_cContext.cLock.lock();

        if (!l_iProxy->RegisterCpu(l_bI, false, GetPerformanceFrequency(), l_pChannel, 128*1024, true, l_pFifo))
        {
            return false;
        }

        std::thread *l_cThread = new std::thread([&l_cContext, l_pFifo, l_hModule1, l_iTraces]
        {
            stReadContext l_stReadCtx = {0};
            l_stReadCtx.pFifo = l_pFifo;

            for (int l_iI = 0; l_iI < l_iTraces; l_iI++)
            {
                uP7ProcessIncomingData(&l_stReadCtx, 0);
                uP7TRC(47, l_hModule1, "Iter %d", l_iI);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
        
        l_cContext.cMap.insert({l_cThread->get_id(), l_pFifo});
        l_cContext.cLock.unlock();

        l_cThreads.push_back(l_cThread);
        l_cFifos.push_back(l_pFifo);
    }

    for (uint8_t l_bI = 0; l_bI < i_bCount; l_bI++)
    {
        l_cThreads[l_bI]->join();
        delete l_cThreads[l_bI];
    }

    int l_iTimeout  = 250;
    int l_iWaitTime = 5000;
    while (    (((uint64_t)l_iTraces * (uint64_t)i_bCount) != l_cP7Sink.GetCount())
            && (l_iWaitTime > 0)
          )
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(l_iTimeout));
        l_iWaitTime -= l_iTimeout;
    }

    printf("Time = %d\n", l_iWaitTime);

    for (uint8_t l_bI = 0; l_bI < i_bCount; l_bI++)
    {
        l_cFifos[l_bI]->Release();
        l_iProxy->UnRegisterCpu(l_bI);
    }

    uP7unInitialize();

    if (l_iProxy)
    {
        l_iProxy->Release();
        l_iProxy = NULL;
    }

    if (    (((uint64_t)l_iTraces * (uint64_t)i_bCount) != l_cP7Sink.GetCount())
         || (l_cP7Sink.GetErrorsCount())
       )
    {
        return false;
    }

    return true;
}
