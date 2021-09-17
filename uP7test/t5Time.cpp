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
class C5TimeShift : public IuP7Time
{
    uint64_t m_qwFreq;
    uint64_t m_qwTimeStart;
    uint64_t m_qwTicksPerSecondShift;
    std::atomic<int> m_iRc;
public:
    C5TimeShift(uint64_t i_qwMsPerSecondShift)
        : m_qwFreq(GetPerformanceFrequency())
        , m_qwTimeStart(GetPerformanceCounter())
        , m_qwTicksPerSecondShift(0)
        , m_iRc(1)
    {
        m_qwTicksPerSecondShift = i_qwMsPerSecondShift * m_qwFreq / 1000ull;
    }

    uint64_t GetClockValue() //artificially shift time
    {
        double l_dbDurationSec = (double)(GetPerformanceCounter() - m_qwTimeStart) / (double)m_qwFreq;
        return GetPerformanceCounter() + (uint64_t)(l_dbDurationSec * (double)m_qwTicksPerSecondShift);
        return GetPerformanceCounter();
    }

    virtual uint64_t GetFrequency()
    {
        return m_qwFreq;
    }

    virtual uint64_t GetTime()
    {
        return GetClockValue();
    }

    virtual int32_t Add_Ref() 
    {
        return ++m_iRc;
    }

    virtual int32_t Release()
    {
        if (0 == --m_iRc)
        {
            delete this;
            return 0;
        }
        return m_iRc;
    }
private:
    virtual ~C5TimeShift()
    {

    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CT5P7Sink: public CClTextSink
{
    size_t      m_szErrors;
    std::mutex  m_cLock;
    uint64_t    m_qwStartTime;
    uint64_t    m_qwEndtTime;
public:
    CT5P7Sink()
    {
        m_szErrors    = 0;
        m_qwStartTime = 0ul;
        m_qwEndtTime  = 0ull;
    }

    virtual ~CT5P7Sink()
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
        if (0 != PStrICmp(TM("MicroP7"), i_rRawLog.pChannel))
        {
            if (!m_qwStartTime)
            {
                m_qwStartTime = i_rRawLog.qwRawTime;
            }

            m_qwEndtTime = i_rRawLog.qwRawTime;
        }
        else 
        {
            if (i_rRawLog.eLevel > EP7TRACE_LEVEL_ERROR)
            {
                m_szErrors ++;
            }
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

    uint64_t GetDurationMilliseconds()
    {
        m_cLock.lock();
        uint64_t l_qwReturn = (m_qwEndtTime - m_qwStartTime) / 10000;
        m_cLock.unlock();

        return l_qwReturn;
    }

    size_t GetErrorsCount()
    {
        m_cLock.lock();
        size_t l_szReturn = m_szErrors;
        m_cLock.unlock();
        return l_szReturn;
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////uP7 callbacks/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint64_t t5getTimerFrequency(void *i_pCtx)      { UNUSED_ARG(i_pCtx); return GetPerformanceFrequency();     }
uint64_t t5getTimerValue(void *i_pCtx)          { return static_cast<C5TimeShift*>(i_pCtx)->GetClockValue();}
uint32_t t5getCurrentThreadId(void *i_pCtx)     { UNUSED_ARG(i_pCtx); return CProc::Get_Thread_Id();        }
bool     t5sendPacket(void *i_pCtx, enum euP7packetRank i_eRank, const struct stP7packetChunk *i_pChunks,
                      size_t  i_szChunks, size_t i_szData)
{
    UNUSED_ARG(i_eRank);
    IuP7Fifo *l_pFifo = static_cast<IuP7Fifo*>(i_pCtx);

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

    return (0 == i_szChunks);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool t5Time(const tXCHAR *i_pSessionFolder)
{
    C5TimeShift*l_pTimeShift = new C5TimeShift(100);
    CT5P7Sink   l_cP7Sink; 
    uint64_t    l_qwAdd = (uint64_t)(&l_cP7Sink);
    tXCHAR      l_pArgs[4096];

    PSPrint(l_pArgs, LENGTH(l_pArgs), TM("/P7.Verb=0 /P7.Sink=ExternalSinc /P7.ExtAddr=%llX /P7.Pool=16384"), l_qwAdd);
    //PSPrint(l_pArgs, LENGTH(l_pArgs), TM("/P7.Verb=0 /P7.Sink=Baical /P7.Pool=16384"), l_qwAdd);

    IuP7proxy *l_iProxy = uP7createProxy(l_pArgs, i_pSessionFolder);
    IuP7Fifo  *l_iFifo  = NULL; 

    if (!l_iProxy->RegisterCpu(0, false, l_pTimeShift, TM("T5"), 128*1024, true, l_iFifo))
    {
        return false;
    }

    struct stuP7config l_stConfig; 

    l_stConfig.bSessionIdCrc7        = g_bCrc7;               /**< Session ID crc */ 
    l_stConfig.uSessionId            = g_uSessionId;          /**< Session ID*/  
    l_stConfig.pCtxTimer             = l_pTimeShift;          /**< Context for timer functions */
    l_stConfig.cbTimerFrequency      = t5getTimerFrequency;   /**< callback to get system high-resolution timer frequency */
    l_stConfig.cbTimerValue          = t5getTimerValue;       /**< callback to get system high-resolution timer value */
    l_stConfig.pCtxLock              = NULL;                  /**< Context for lock functions, set it to NULL if there is no RTOS*/
    l_stConfig.cbLock                = NULL;                  /**< callback to call OS lock function, set it to NULL if there is no RTOS */
    l_stConfig.cbUnLock              = NULL;                  /**< callback to call OS unlock function, set it to NULL if there is no RTOS */
    l_stConfig.pCtxPacket            = l_iFifo;               /**< Context for data sending */
    l_stConfig.cbSendPacket          = t5sendPacket;          /**< callback to send data packet to host */  
    l_stConfig.pCtxThread            = 0;                     /**< Context for cbGetCurrentThreadId function */
    l_stConfig.cbGetCurrentThreadId  = t5getCurrentThreadId;  /**< callback get current thread ID, set it to NULL if there is no RTOS*/  
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
    if (    (!uP7TelCreateCounter("t5 Counter", 0, 0, 1, 1, true, &l_hCounter1))
         || (!uP7TrcRegisterModule("t5", euP7Level_Trace, &l_hModule1))
       )
    {
        return false;
    }

    std::thread *l_pThread = new std::thread([&]
    {
        stReadContext l_stReadCtx = {0};
        l_stReadCtx.pFifo = l_iFifo;
        uint32_t l_uTimeStart = GetTickCount();

        while (CTicks::Difference(GetTickCount(), l_uTimeStart) < T5_DURATION_MS)
        {
            uP7ProcessIncomingData(&l_stReadCtx, 10);
            uP7TRC(48, l_hModule1, "T5 test message", 0);
            //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });


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


    if (l_cP7Sink.GetErrorsCount())
    {
        return false;
    }

    //if drift is more than 250ms
    uint64_t qwTestDuration = l_cP7Sink.GetDurationMilliseconds();
    int32_t  iDrift = (int32_t)qwTestDuration - T5_DURATION_MS;

    printf("t5Time: drift: %d\n", iDrift);

    if (abs(iDrift) > 250)
    {
        return false;
    }

    l_pTimeShift->Release();
    l_pTimeShift = NULL;

    return true;
}
