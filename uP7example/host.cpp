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
#include <stdio.h>    

extern "C"
{
    #include "uP7.h" 
}

#include "cpu.h"
#include "uP7proxyApi.h"
#include <chrono>
#include <thread>


IuP7Fifo *g_pFifo  = NULL;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function is emulating CPU FIFO access
bool WriteFifo(const struct stP7packetChunk *i_pChunks, size_t i_szChunks, size_t i_szData)
{
    if (g_pFifo->GetFreeBytes() < i_szData)
    {
        return false;
    }

    while (i_szChunks--)
    {
        g_pFifo->Write(i_pChunks->pData, i_pChunks->szData);
        i_pChunks++;

    }

    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function is emulating CPU FIFO access
size_t ReadFifo(void *o_pData, size_t i_szData)
{
    return g_pFifo->Read(o_pData, i_szData, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32) || defined(_WIN64)
int wmain(int i_iArgC, tXCHAR *i_pArgV[])
#elif defined(__linux__)
int main(int i_iArgC, tXCHAR *i_pArgV[])
#endif
{
    if (i_iArgC < 2)
    {
        printf("Usage example: uP7example�<uP7Lib�folder>/Platforms/x86example\n");
        printf("Proxy libary need *.up7 files to be able to parse incoming uP7 packets.\n");
        printf("Files like *.up7 are generated by uP7 pre processor\n");
        return -1;
    }

    IuP7proxy *l_iProxy = uP7createProxy(TM("/P7.Verb=0 /P7.Sink=Baical /P7.Pool=1024"), i_pArgV[1]);

    l_iProxy->RegisterCpu(1, false, CpuTimerFrequency(), TM("CPU simulation example"), 0xFFFF, true, g_pFifo);

    CpuStart(); 


    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    l_iProxy->UnRegisterCpu(1);

    if (g_pFifo)
    {
        g_pFifo->Release();
        g_pFifo = NULL;
    }

    if (l_iProxy)
    {
        l_iProxy->Release();
        l_iProxy = NULL;
    }
 
    return 0;
}

