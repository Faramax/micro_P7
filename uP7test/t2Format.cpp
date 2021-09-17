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
class CT2P7Sink: public CClTextSink
{
    const tXCHAR      *m_pChannel;
    CBList<CWString *> m_cText;
    size_t             m_szErrors;
    std::mutex         m_cLock;
public:

    CT2P7Sink(const tXCHAR *i_pChannel)
        : m_pChannel(i_pChannel)
        , m_szErrors(0)
    {

    }

    virtual ~CT2P7Sink()
    {
        m_cText.Clear(TRUE);
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
        if (0 == PStrICmp(m_pChannel, i_rRawLog.pChannel))
        {
            m_cText.Push_Last(new CWString(i_rRawLog.pMessage));
        }
        else if (i_rRawLog.eLevel > EP7TRACE_LEVEL_ERROR)
        {
            m_szErrors ++;
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

    size_t GetErrorsCount()
    {
        m_cLock.lock();
        size_t l_szReturn = m_szErrors;
        m_cLock.unlock();

        return l_szReturn;
    }

    bool IsPattern(const tXCHAR *i_pText)
    {
        bool l_bReturn = false;

        m_cLock.lock();
        if (m_cText.Count())
        {
            CWString *l_pText = m_cText.Pull_First();

            if (l_pText)
            {
                if (0 == PStrICmp(l_pText->Get(), i_pText))
                {
                   l_bReturn = true; 
                }
                else
                {
                    l_bReturn = false;
                }

                delete l_pText;
            }
        }
        m_cLock.unlock();

        if (!l_bReturn)
        {
            printf("Panic");
        }

        return l_bReturn;
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////uP7 callbacks/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint64_t t2getTimerFrequency(void *i_pCtx)      { UNUSED_ARG(i_pCtx); return GetPerformanceFrequency(); }
uint64_t t2getTimerValue(void *i_pCtx)          { UNUSED_ARG(i_pCtx); return GetPerformanceCounter();   }
uint32_t t2getCurrentThreadId(void *i_pCtx)     { UNUSED_ARG(i_pCtx); return CProc::Get_Thread_Id();    }
bool     t2sendPacket(void *i_pCtx, enum euP7packetRank i_eRank, const struct stP7packetChunk *i_pChunks,
                      size_t  i_szChunks, size_t i_szData)
{
    UNUSED_ARG(i_eRank);

    IuP7Fifo  *l_pFifo = static_cast<IuP7Fifo  *>(i_pCtx);

    if (l_pFifo->GetFreeBytes() < i_szData)
    {
        return false;
    }

    while (i_szChunks--)
    {
        l_pFifo->Write(i_pChunks->pData, i_pChunks->szData);
        i_pChunks++;

    }

    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool t2Format(const tXCHAR *i_pSessionFolder)
{
    const uint8_t l_bCpuId   = 1;
    const tXCHAR *l_pChannel = TM("t2Format");
    CT2P7Sink     l_cP7Sink(l_pChannel); 
    uint64_t      l_qwAdd = (uint64_t)(&l_cP7Sink);
    tXCHAR        l_pArgs[4096];

    PSPrint(l_pArgs, 4096, TM("/P7.Verb=0 /P7.Sink=ExternalSinc /P7.ExtAddr=%llX /P7.Eto=0 /P7.Pool=4096"), l_qwAdd);

    IuP7proxy *l_iProxy = uP7createProxy(l_pArgs, i_pSessionFolder);
    IuP7Fifo  *l_pFifo  = NULL;

    if (!l_iProxy->RegisterCpu(l_bCpuId, false, GetPerformanceFrequency(), l_pChannel, 512*1024, true, l_pFifo))
    {
        return false;
    }


    struct stuP7config l_stConfig; 

    l_stConfig.bSessionIdCrc7        = g_bCrc7;               /**< Session ID crc */ 
    l_stConfig.uSessionId            = g_uSessionId;          /**< Session ID*/  
    l_stConfig.pCtxTimer             = NULL;                  /**< Context for timer functions */
    l_stConfig.cbTimerFrequency      = t2getTimerFrequency;   /**< callback to get system high-resolution timer frequency */
    l_stConfig.cbTimerValue          = t2getTimerValue;       /**< callback to get system high-resolution timer value */
    l_stConfig.pCtxLock              = NULL;                  /**< Context for lock functions, set it to NULL if there is no RTOS*/
    l_stConfig.cbLock                = NULL;                  /**< callback to call OS lock function, set it to NULL if there is no RTOS */
    l_stConfig.cbUnLock              = NULL;                  /**< callback to call OS unlock function, set it to NULL if there is no RTOS */
    l_stConfig.pCtxPacket            = l_pFifo;               /**< Context for data sending */
    l_stConfig.cbSendPacket          = t2sendPacket;          /**< callback to send data packet to host */  
    l_stConfig.pCtxThread            = 0;                     /**< Context for cbGetCurrentThreadId function */
    l_stConfig.cbGetCurrentThreadId  = t2getCurrentThreadId;  /**< callback get current thread ID, set it to NULL if there is no RTOS*/  
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

    huP7Module l_hModule1  = uP7_TELEMETRY_INVALID_ID;
    huP7TelId  l_hCounter1 = uP7_TELEMETRY_INVALID_ID;
    if (    (!uP7TelCreateCounter("t2 Counter", 0, 0, 1, 1, true, &l_hCounter1))
         || (!uP7TrcRegisterModule("t2", euP7Level_Trace, &l_hModule1))
       )
    {
        return false;
    }
    
    volatile bool l_bExit = false;
    std::thread l_hThread = std::thread([&] 
    {
        stReadContext l_stReadCtx = {0};
        l_stReadCtx.pFifo = l_pFifo;

        while (!l_bExit)
        {
            uP7ProcessIncomingData(&l_stReadCtx, 10);
        }
    });

    uP7TRC(2, l_hModule1, "String: %.10ls (%%.10ls = ([x]s is unsupported by uP7))", L"12345 abcde .. z // \\ ABCDE .. Z");
    uP7TRC(5, l_hModule1, "String: %s (%%s = 12345 abcde .. z // \\ ABCDE .. Z)", "12345 abcde .. z // \\ ABCDE .. Z");
    uP7TRC(0, l_hModule1, "String: %.*s (%%.*s = ([x]s is unsupported by uP7))", 0, "12345 abcde .. z // \\ ABCDE .. Z");
    uP7TRC(1, l_hModule1, "String: %ls (%%ls = 12345 abcde .. z // \\ ABCDE .. Z)", L"12345 abcde .. z // \\ ABCDE .. Z");
    uP7TRC(3, l_hModule1, "String: %hs (%%hs = 12345 abcde .. z // \\ ABCDE .. Z)", "12345 abcde .. z // \\ ABCDE .. Z");
    uP7TRC(4, l_hModule1, "String: %.10hs (%%.10hs = ([x]s is unsupported by uP7))", "12345 abcde .. z // \\ ABCDE .. Z");
    uP7TRC(6, l_hModule1, "String: %ws (%%ws = 12345 abcde .. z // \\ ABCDE .. Z)", L"12345 abcde .. z // \\ ABCDE .. Z");
    uP7TRC(7, l_hModule1, "String: %.10ws (%%.10ws = ([x]s is unsupported by uP7))", L"12345 abcde .. z // \\ ABCDE .. Z");

    uP7TRC(8, l_hModule1, "String: %s (%%s = NULL)", NULL);
    uP7TRC(9, l_hModule1, "String: %.10s (%%.10s = ([x]s is unsupported by uP7))", NULL);
    uP7TRC(10, l_hModule1, "String: %hs (%%hs = NULL)", NULL);
    uP7TRC(11, l_hModule1, "String: %ls (%%ls = NULL)", NULL);
    uP7TRC(12, l_hModule1, "String: %s (%%s = \"\")", "");
    uP7TRC(13, l_hModule1, "String: %hs (%%hs = \"\")", "");
    uP7TRC(14, l_hModule1, "String: %ls (%%ls = \"\")", L"");

    uP7TRC(15, l_hModule1, "Char: %c (%%c = G)", 'G');
    uP7TRC(16, l_hModule1, "Char: %hc (%%hc = G)", 'G');
    uP7TRC(17, l_hModule1, "Char: %C (%%C = F)", 'F');
    uP7TRC(18, l_hModule1, "Char: %hC (%%hC = R)", 'R');
    uP7TRC(19, l_hModule1, "Char: %lc (%%lc = S)", L'S');
    uP7TRC(20, l_hModule1, "Char: %wc (%%wc = Z)", L'Z');



    uP7TRC(21, l_hModule1, "Decamical %d (%%d = 1234567890) | %i (%%i = 1234567890) | %u (%%u = 1234567890) ", 
                          (tINT32)1234567890,
                          (tINT32)1234567890,
                          (tUINT32)1234567890
                         );

    uP7TRC(22, l_hModule1, "Decamical %jd (%%jd = 1234567890) | %ji (%%ji = 1234567890) | %ju (%%ju = 1234567890) ", 
                          (intmax_t)1234567890,
                          (intmax_t)1234567890,
                          (uintmax_t)1234567890
                         );

    uP7TRC(23, l_hModule1, "Octal  %o (%%o = 11145401322) | Octal  %jo (%%jo = 11145401322)", 
                          (tUINT32)1234567890,
                          (uintmax_t)1234567890
                         );

    uP7TRC(24, l_hModule1, "Hex  %x (%%x = 499602d2) | %X (%%X = 499602D2), %jX (%%jX = 499602D2)", 
                          (tUINT32)1234567890,
                          (tUINT32)1234567890,
                          (uintmax_t)1234567890
                         );

    uP7TRC(25, l_hModule1, "Decamical %ld (%%ld = 1234567890) | %li (%%li = 1234567890) | %lu (%%lu = 1234567890) ", 
                          (long int)1234567890,
                          (long int)1234567890,
                          (unsigned long int)1234567890
                         );

    uP7TRC(26, l_hModule1, "Octal  %lo (%%lo = 11145401322)", 
                          (unsigned long int)1234567890
                         );


    uP7TRC(27, l_hModule1, "Hex  %lx (%%lx = 499602d2) | %lX (%%lX = 499602D2)", 
                          (unsigned long int)1234567890,
                          (unsigned long int)1234567890
                         );


    uP7TRC(28, l_hModule1, "Decamical %lld (%%lld = 1234567890987) | %lli (%%lli = 1234567890987ULL)", 
                          (tINT64)1234567890987ULL,
                          (tINT64)1234567890987ULL
                         );


    uP7TRC(29, l_hModule1, "Octal  %llo (%%llo = 21756176604053ULL)", 
                          (tUINT64)1234567890987ULL
                         );


    uP7TRC(30, l_hModule1, "Hex  %llx (%%llx = ffaabbccee12) | %llX (%%llX = FFAABBCCEE12)", 
                          (tUINT64)0xffaabbccee12ULL,
                          (tUINT64)0xFFAABBCCEE12ULL
                         );



    uP7TRC(31, l_hModule1, "Decamical %hd (%%hd = 32700) | %hi (%%hi = 32700) | %hu (%%hu = 65500) ", 
                          (tINT16)32700,
                          (tINT16)32700,
                          (tUINT16)65500
                         );

    uP7TRC(32, l_hModule1, "Octal  %ho (%%ho = 77674)", (tUINT16)32700);
    uP7TRC(33, l_hModule1, "Hex  %hx (%%hx = AABB) | %hX (%%hX = AABB)", (tUINT16)0xAABB, (tUINT16)0xAABB);

    uP7TRC(34, l_hModule1, "Decamical %I32d (%%I32d = 1234567890) | %I32i (%%I32i = 1234567890) | %I32u (%%I32u = 1234567890) ", 
                          (tINT32)1234567890,
                          (tINT32)1234567890,
                          (tUINT32)1234567890
                         );

    uP7TRC(35, l_hModule1, "Octal  %I32o (%%I32o = 11145401322)", (tUINT32)1234567890);


    uP7TRC(36, l_hModule1, "Hex  %I32x (%%I32x = 499602d2) | %I32X (%%I32X = 499602D2)", 
                          (tUINT32)1234567890,
                          (tUINT32)1234567890
                         );

    uP7TRC(37, l_hModule1, "Decamical %I64d (%%I64d = 1234567890987) | %I64i (%%I64i = 1234567890987)", 
                          (tINT64)1234567890987ULL,
                          (tINT64)1234567890987ULL
                         );

    uP7TRC(38, l_hModule1, "Octal  %I64o (%%I64o = 21756176604053)", 
                          (tUINT64)1234567890987ULL
                         );

    uP7TRC(39, l_hModule1, "Hex  %llx (%%llx = ffaabbccee12) | %llX (%%llX = FFAABBCCEE12)",
                          (tUINT64)0xffaabbccee12ULL,
                          (tUINT64)0xFFAABBCCEE12ULL
                         );

    uP7TRC(40, l_hModule1, "Bin %b (%%b = 10101010101010101) | %#b (%%#b = b10101010101010101)",
                          (tUINT32)0x15555,
                          (tUINT32)0x15555
                         );
    uP7TRC(41, l_hModule1, "%f (%%f 123456.7890) | %e (%%e) | %E (%%E) | %G (%%G) | %g (%%g) | %A (%%A) | %a (%%a)", 
                          (tDOUBLE)123456.7890,
                          (tDOUBLE)123456.7890,
                          (tDOUBLE)123456.7890,
                          (tDOUBLE)123456.7890,
                          (tDOUBLE)123456.7890,
                          (tDOUBLE)123456.7890,
                          (tDOUBLE)123456.7890
                         );


    uP7TRC(42, l_hModule1, "%s (String1), %.*s ([x]s is unsupported by uP7) %d(1), %d(2), %d(3), %d(4) "
                           "%I64X(ABCDEF12345678) %d(5), %d(6), %d(7), %d(8) "
                           "%llx(abcdef12345678) %d(9), %d(10), %d(11), %d(12) "
                           "%f(987654321.12345), %d(13), %jd(100500), %d(14), %d(15), %d(16) "
                           "%c(X) %d(17), %d(18), %d(19), %d(20) "
                           "%hd(32000) %d(21), %d(22), %d(23), %d(24) "
                           "%s(String2) %d(25), %d(26), %d(27), %d(28), %.04f(123456.7890), % 5lld(   30)",
                           "String1",
                           100,
                           "String2",
                           (tINT32)1,
                           (tINT32)2,
                           (tINT32)3,
                           (tINT32)4,
                           (tUINT64)0xABCDEF12345678ULL,
                           (tINT32)5,
                           (tINT32)6,
                           (tINT32)7,
                           (tINT32)8,
                           (tUINT64)0xABCDEF12345678ULL,
                           (tINT32)9,
                           (tINT32)10,
                           (tINT32)11,
                           (tINT32)12,
                           (tDOUBLE)987654321.12345,
                           (tINT32)13,
                           (uintmax_t)100500,
                           (tINT32)14,
                           (tINT32)15,
                           (tINT32)16,
                           'X',
                           (tINT32)17,
                           (tINT32)18,
                           (tINT32)19,
                           (tINT32)20,
                           (tINT16)32000,
                           (tINT32)21,
                           (tINT32)22,
                           (tINT32)23,
                           (tINT32)24,
                           "String2",
                           (tINT32)25,
                           (tINT32)26,
                           (tINT32)27,
                           (tINT32)28,
                           (tDOUBLE)123456.789,
                           (tUINT64)30ull
                         );


        uP7TRC(43, l_hModule1, "Hex  %#X (%%#X = 0xDEADBEEF) | {%#*X} (%%#*X = {  0xDEADBEEF}})",
                          (tUINT32)0xDEADBEEF,
                          12,
                          (tUINT32)0xDEADBEEF
                         );

        uP7TRC(44, l_hModule1, "Hex  %#012X (%%#012X = 0x00DEADBEEF) | {%#-*.*X} (%%#-14.10X = {0x00DEADBEEF  }})",
                          (tUINT32)0xDEADBEEF,
                          14,
                          10,
                          (tUINT32)0xDEADBEEF
                         );

        uP7TRC(45, l_hModule1, "Hex  {%+10d} (%%+10d = {  +1234567}) | {%+*.*d} (%%+*.*d = { +0001234567}})",
                          (tUINT32)1234567,
                          12,
                          10,
                          (tUINT32)1234567
                         );

    std::this_thread::sleep_for(std::chrono::seconds(2));

    l_bExit = true;
    l_hThread.join();

    uP7unInitialize();

    if (l_pFifo)
    {
        l_pFifo->Release();
        l_pFifo = NULL;
    }

    if (l_iProxy)
    {
        l_iProxy->UnRegisterCpu(l_bCpuId);
        l_iProxy->Release();
        l_iProxy = NULL;
    }
 
    //analyze the result
    if (    (l_cP7Sink.GetErrorsCount())

         || (!l_cP7Sink.IsPattern(TM("String: ([x]s is unsupported by uP7) (%.10ls = ([x]s is unsupported by uP7))")))
         || (!l_cP7Sink.IsPattern(TM("String: 12345 abcde .. z // \\ ABCDE .. Z (%s = 12345 abcde .. z // \\ ABCDE .. Z)")))
         || (!l_cP7Sink.IsPattern(TM("String: ([x]s is unsupported by uP7) (%.*s = ([x]s is unsupported by uP7))")))
         || (!l_cP7Sink.IsPattern(TM("String: 12345 abcde .. z // \\ ABCDE .. Z (%ls = 12345 abcde .. z // \\ ABCDE .. Z)")))
         || (!l_cP7Sink.IsPattern(TM("String: 12345 abcde .. z // \\ ABCDE .. Z (%hs = 12345 abcde .. z // \\ ABCDE .. Z)")))
         || (!l_cP7Sink.IsPattern(TM("String: ([x]s is unsupported by uP7) (%.10hs = ([x]s is unsupported by uP7))")))
         || (!l_cP7Sink.IsPattern(TM("String: 12345 abcde .. z // \\ ABCDE .. Z (%ws = 12345 abcde .. z // \\ ABCDE .. Z)")))
         || (!l_cP7Sink.IsPattern(TM("String: ([x]s is unsupported by uP7) (%.10ws = ([x]s is unsupported by uP7))")))
         || (!l_cP7Sink.IsPattern(TM("String: (NULL) (%s = NULL)")))
         || (!l_cP7Sink.IsPattern(TM("String: ([x]s is unsupported by uP7) (%.10s = ([x]s is unsupported by uP7))")))
         || (!l_cP7Sink.IsPattern(TM("String: (NULL) (%hs = NULL)")))
         || (!l_cP7Sink.IsPattern(TM("String: (NULL) (%ls = NULL)")))
         || (!l_cP7Sink.IsPattern(TM("String:  (%s = \"\")")))
         || (!l_cP7Sink.IsPattern(TM("String:  (%hs = \"\")")))
         || (!l_cP7Sink.IsPattern(TM("String:  (%ls = \"\")")))

         || (!l_cP7Sink.IsPattern(TM("Char: G (%c = G)")))
         || (!l_cP7Sink.IsPattern(TM("Char: G (%hc = G)")))
         || (!l_cP7Sink.IsPattern(TM("Char: F (%C = F)")))
         || (!l_cP7Sink.IsPattern(TM("Char: R (%hC = R)")))
         || (!l_cP7Sink.IsPattern(TM("Char: S (%lc = S)")))
         || (!l_cP7Sink.IsPattern(TM("Char: Z (%wc = Z)")))


         || (!l_cP7Sink.IsPattern(TM("Decamical 1234567890 (%d = 1234567890) | 1234567890 (%i = 1234567890) | 1234567890 (%u = 1234567890) ")))
         || (!l_cP7Sink.IsPattern(TM("Decamical 1234567890 (%jd = 1234567890) | 1234567890 (%ji = 1234567890) | 1234567890 (%ju = 1234567890) ")))
         || (!l_cP7Sink.IsPattern(TM("Octal  11145401322 (%o = 11145401322) | Octal  11145401322 (%jo = 11145401322)")))
         || (!l_cP7Sink.IsPattern(TM("Hex  499602d2 (%x = 499602d2) | 499602D2 (%X = 499602D2), 499602D2 (%jX = 499602D2)")))
         || (!l_cP7Sink.IsPattern(TM("Decamical 1234567890 (%ld = 1234567890) | 1234567890 (%li = 1234567890) | 1234567890 (%lu = 1234567890) ")))
         || (!l_cP7Sink.IsPattern(TM("Octal  11145401322 (%lo = 11145401322)")))
         || (!l_cP7Sink.IsPattern(TM("Hex  499602d2 (%lx = 499602d2) | 499602D2 (%lX = 499602D2)")))
         || (!l_cP7Sink.IsPattern(TM("Decamical 1234567890987 (%lld = 1234567890987) | 1234567890987 (%lli = 1234567890987ULL)")))
         || (!l_cP7Sink.IsPattern(TM("Octal  21756176604053 (%llo = 21756176604053ULL)")))
         || (!l_cP7Sink.IsPattern(TM("Hex  ffaabbccee12 (%llx = ffaabbccee12) | FFAABBCCEE12 (%llX = FFAABBCCEE12)")))
         || (!l_cP7Sink.IsPattern(TM("Decamical 32700 (%hd = 32700) | 32700 (%hi = 32700) | 65500 (%hu = 65500) ")))
         || (!l_cP7Sink.IsPattern(TM("Octal  77674 (%ho = 77674)")))
         || (!l_cP7Sink.IsPattern(TM("Hex  AABB (%hx = AABB) | AABB (%hX = AABB)")))
         || (!l_cP7Sink.IsPattern(TM("Decamical 1234567890 (%I32d = 1234567890) | 1234567890 (%I32i = 1234567890) | 1234567890 (%I32u = 1234567890) ")))
         || (!l_cP7Sink.IsPattern(TM("Octal  11145401322 (%I32o = 11145401322)")))
         || (!l_cP7Sink.IsPattern(TM("Hex  499602d2 (%I32x = 499602d2) | 499602D2 (%I32X = 499602D2)")))
         || (!l_cP7Sink.IsPattern(TM("Decamical 1234567890987 (%I64d = 1234567890987) | 1234567890987 (%I64i = 1234567890987)")))
         || (!l_cP7Sink.IsPattern(TM("Octal  21756176604053 (%I64o = 21756176604053)")))
         || (!l_cP7Sink.IsPattern(TM("Hex  ffaabbccee12 (%llx = ffaabbccee12) | FFAABBCCEE12 (%llX = FFAABBCCEE12)")))
         || (!l_cP7Sink.IsPattern(TM("Bin 10101010101010101 (%b = 10101010101010101) | b10101010101010101 (%#b = b10101010101010101)")))
         || (!l_cP7Sink.IsPattern(TM("123456.789000 (%f 123456.7890) | 1.234568e+05 (%e) | 1.234568E+05 (%E) | 123457 (%G) | 123457 (%g) | 0X1.E240C9FBE76C9P+16 (%A) | 0x1.e240c9fbe76c9p+16 (%a)")))
         || (!l_cP7Sink.IsPattern(TM("String1 (String1), ([x]s is unsupported by uP7) ([x]s is unsupported by uP7) 1(1), 2(2), 3(3), 4(4) ABCDEF12345678(ABCDEF12345678) 5(5), 6(6), 7(7), 8(8) abcdef12345678(abcdef12345678) 9(9), 10(10), 11(11), 12(12) 987654321.123450(987654321.12345), 13(13), 100500(100500), 14(14), 15(15), 16(16) X(X) 17(17), 18(18), 19(19), 20(20) 32000(32000) 21(21), 22(22), 23(23), 24(24) String2(String2) 25(25), 26(26), 27(27), 28(28), 123456.7890(123456.7890),    30(   30)")))

         || (!l_cP7Sink.IsPattern(TM("Hex  0xDEADBEEF (%#X = 0xDEADBEEF) | {  0xDEADBEEF} (%#*X = {  0xDEADBEEF}})")))
         || (!l_cP7Sink.IsPattern(TM("Hex  0x00DEADBEEF (%#012X = 0x00DEADBEEF) | {0x00DEADBEEF  } (%#-14.10X = {0x00DEADBEEF  }})")))
         || (!l_cP7Sink.IsPattern(TM("Hex  {  +1234567} (%+10d = {  +1234567}) | { +0001234567} (%+*.*d = { +0001234567}})")))
       )
    {
        return false;
    }


    return true;
}
