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
#ifndef UP7_TEST_COMMON_H
#define UP7_TEST_COMMON_H

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include <atomic>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <map>

#include "GTypes.h"
#include "Length.h"
#include "AList.h"
#include "RBTree.h"
#include "PString.h"
#include "UTF.h"
#include "WString.h"
#include "PConsole.h"
#include "PAtomic.h"
#include "PTime.h"
#include "PFileSystem.h"
#include "PProcess.h"
#include "PSignal.h"
#include "PLock.h"
#include "Lock.h"
#include "IMEvent.h"
#include "PMEvent.h"
#include "PThreadShell.h"
#include "PSystem.h"
#include "Ticks.h"
#include "IFile.h"
#include "PFile.h"
#include "P7_Version.h"
#include "P7_Client.h"
#include "P7_Trace.h"
#include "P7_Telemetry.h"
#include "P7_Extensions.h"
#include "ClTextSink.h"

extern "C" 
{
    #include "uP7.h"
}

#include "uP7proxyApi.h"

#include "t1CreateDestroy.h"
#include "t2Format.h"
#include "t3Performance.h"
#include "t4MultiCore.h"
#include "t5Time.h"
#include "t6Resync.h"

extern uint32_t          g_uSessionId;
extern uint8_t           g_bCrc7;
extern size_t            g_szModules;
extern stuP7Module       g_pModules[];
extern size_t            g_szTelemetry;
extern stuP7telemetry    g_pTelemetry[];
extern size_t            g_szTraces;
extern struct stuP7Trace g_pTraces[];


struct stReadContext
{
    const static size_t szDataIn = 4096;
    uint8_t             pDataIn[szDataIn];
    size_t              szDataInTail;
    IuP7Fifo           *pFifo;

};


static UNUSED_FUNC size_t uP7ProcessIncomingData(stReadContext *pContext, uint32_t i_uTimeout)
{
    //Microprocessor code is reading incoming data for uP7
    size_t l_szData      = pContext->pFifo->Read(pContext->pDataIn + pContext->szDataInTail, stReadContext::szDataIn - pContext->szDataInTail, i_uTimeout);
    size_t l_szProcessed = 0;
           
    if (l_szData) //if data has been read
    {
        l_szData += pContext->szDataInTail; //if there was data tail since last call - we are increasing data size by tail
        pContext->szDataInTail = 0;

        l_szProcessed = uP7process(pContext->pDataIn, l_szData); //feed uP7 with received data
        if (l_szProcessed < l_szData) //if not all data were processed by uP7 - we will keep the reminder for next time
        {
            pContext->szDataInTail = l_szData - l_szProcessed;
            memmove(pContext->pDataIn, pContext->pDataIn + l_szProcessed, pContext->szDataInTail);
        }
    }

    return l_szProcessed;
}


#endif //UP7_TEST_COMMON_H