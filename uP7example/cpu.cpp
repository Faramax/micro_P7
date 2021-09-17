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
#include <stdlib.h>     /* srand, rand */                                

#include "GTypes.h"
#include "AList.h"
#include "PString.h"
#include "UTF.h"
#include "PTime.h"
#include "WString.h"
#include "PConsole.h"
#include "PProcess.h"
#include "PTime.h"
#include "fifo.h"

extern "C" 
{
    #include "uP7.h"
}

#include "uP7preprocessed.h"
#include "uP7IDs.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//provide hi-resolution, monotonic timer frequency to host. 
uint64_t CpuTimerFrequency()
{
    return GetPerformanceFrequency();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////uP7 callbacks/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//provide to uP7 CPU hi-resolution, monotonic timer frequency
uint64_t P7getTimerFrequency(void *i_pCtx) { UNUSED_ARG(i_pCtx); return GetPerformanceFrequency(); }

//provide to uP7 CPU hi-resolution, monotonic timer value
uint64_t P7getTimerValue(void *i_pCtx) { UNUSED_ARG(i_pCtx); return GetPerformanceCounter();}

//This function is useful only in case of usage RT-OS, otherwise callback should be NULL
uint32_t P7getCurrentThreadId(void *i_pCtx) { UNUSED_ARG(i_pCtx); return CProc::Get_Thread_Id(); }


//Send data to host (uP7 proxy). 
//N.B.: It is important to send either All data or none.
//      For example uP7 will calls the callback with next parameters: 3 chunks, total size 128 bytes.
//      User is checking for free space in transport FIFO and realize that there is only 100 bytes free.
//      In such case there are 2 options
//       * drop the packet (most simple solution, but lead to information loss)
//         For dropping policy and managing FIFO busy level integrator may use i_eRank parameter.
//         For example if rank is low and fifo is full on half - drop every low rank incoming paket.
//         But if rank is hight - try to use fifo for 100%
//       * send 100 bytes and 28 bytes of reminder save in some user buffer and send it right after, when there will be 
//         free space in fifo.
//      If 28 bytes reminder won't be send or will be send in after newly incoming data - communication protocol will be
//      broken and uP7 proxy will stop processing data.
bool P7sendPacket(void                         *i_pCtx,  
                  enum euP7packetRank           i_eRank,  
                  const struct stP7packetChunk *i_pChunks, 
                  size_t                        i_szChunks, 
                  size_t                        i_szData)
{
    UNUSED_ARG(i_pCtx);
    UNUSED_ARG(i_eRank);
    return WriteFifo(i_pChunks, i_szChunks, i_szData);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////uP7 usage/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define INPUT_BUFFER_MAX_SIZE 4096

struct stContext                
{
    huP7Module hModule;     //  = uP7_MODULE_INVALID_ID;
    huP7TelId  hCounter;    //  = uP7_TELEMETRY_INVALID_ID;
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uP7 intialization
void uP7init(stContext *pContext)
{
    struct stuP7config l_stConfig; 

    l_stConfig.bSessionIdCrc7        = g_bCrc7;               /**< Session ID crc */ 
    l_stConfig.uSessionId            = g_uSessionId;          /**< Session ID*/  
    l_stConfig.pCtxTimer             = NULL;                  /**< Context for timer functions */
    l_stConfig.cbTimerFrequency      = P7getTimerFrequency;   /**< callback to get system high-resolution timer frequency */
    l_stConfig.cbTimerValue          = P7getTimerValue;       /**< callback to get system high-resolution timer value */
    l_stConfig.pCtxLock              = NULL;                  /**< Context for lock functions, set it to NULL if there is no RTOS*/
    l_stConfig.cbLock                = NULL;                  /**< callback to call OS lock function, set it to NULL if there is no RTOS */
    l_stConfig.cbUnLock              = NULL;                  /**< callback to call OS unlock function, set it to NULL if there is no RTOS */
    l_stConfig.pCtxPacket            = NULL;                  /**< Context for data sending */
    l_stConfig.cbSendPacket          = P7sendPacket;          /**< callback to send data packet to host */  
    l_stConfig.pCtxThread            = 0;                     /**< Context for cbGetCurrentThreadId function */
    l_stConfig.cbGetCurrentThreadId  = P7getCurrentThreadId;  /**< callback get current thread ID, set it to NULL if there is no RTOS*/  
    l_stConfig.eDefaultVerbosity     = euP7Level_Trace;       /**< Default verbosity*/  

    //Generated descriptors, located in uP7preprocessed.h
    l_stConfig.pModules              = g_pModules;            /**< Trace modules descriptors, generated by uP7 preprocessor*/
    l_stConfig.szModules             = g_szModules;           /**< Trace modules descriptors count*/  
    l_stConfig.pTraces               = g_pTraces;             /**< Trace descriptors, generated by uP7 preprocessor*/  
    l_stConfig.szTraces              = g_szTraces;            /**< Trace descriptors count*/  
    l_stConfig.pTelemetry            = g_pTelemetry;          /**< Telemetry descriptors, generated by uP7 preprocessor*/  
    l_stConfig.szTelemetry           = g_szTelemetry;         /**< Telemetry descriptors count*/  
    l_stConfig.eAlignment            = euP7alignment4;


    uP7initialize(&l_stConfig);

    uP7TrcRegisterModule("Module", euP7Level_Trace, &pContext->hModule);
    uP7TelCreateCounter("CPU/Cycle", 0, 0, 1, 1, true, &pContext->hCounter);

    uP7DBG(0, pContext->hModule, "Initialization is done, start processing", 0); 
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uP7 is capable to recieve data from host and process it. This function is demonstrating such feature
//For example user can change on run-time verbosity level, enable/disable telemetry counter. 
//Due to the memory allocation policy - uP7 DO NOT BUFFER any data inside, to avoid memory allocation/deallocation.
//N.B. function uP7process may process not all incoming data and there will be data remainder, and this is responsability
//     of integrator to keep that reminder and keep data consistency.
//     This function implementation is simple example of such policy.
void uP7ProcessIncomingData(stContext *pContext)
{
    UNUSED_ARG(pContext);

    uint8_t  pDataIn[INPUT_BUFFER_MAX_SIZE];

    //Microprocessor code is reading incoming data for uP7
    size_t l_szData = ReadFifo(pDataIn, INPUT_BUFFER_MAX_SIZE);

    if (l_szData)
    {
        uP7process(pDataIn, l_szData); //feed uP7 with received data
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CpuStart()
{
    stContext l_stCtx  = {};
    uint64_t  l_qwIter = 0;

    double l_dbIter = 0;

    l_stCtx.hModule     = uP7_MODULE_INVALID_ID;
    l_stCtx.hCounter    = uP7_TELEMETRY_INVALID_ID;

    uP7init(&l_stCtx);

    uP7INF(2, uP7M_XML_MODULE, "Start test", 0);

    while (     (!Is_Key_Hit())
             || (27 != Get_Char())
          )
    {
        //using telemetry to signal rising edge (cyclogramm)
        uP7TelSentSample(l_stCtx.hCounter, 1);
        
        uP7ProcessIncomingData(&l_stCtx);

        uP7DBG(1, l_stCtx.hModule, "Iteration %I64d", l_qwIter++);

        uP7TelSentSample(uP7T_XML_COUNTER, l_dbIter);
        l_dbIter += 0.5;
        if (l_dbIter > 100)
        {
            l_dbIter = 0.0;
        }

        //using telemetry to signal failing edge
        uP7TelSentSample(l_stCtx.hCounter, 0);
    }

    uP7INF(3, uP7M_XML_MODULE, "Stop test", 0);
}

