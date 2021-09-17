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

bool t1CreateDestroy(size_t i_szIterations, uint8_t i_szCpus, const tXCHAR *i_pSessionFolder)
{
    bool l_bReturn = true;
    for (size_t i = 0; (i < i_szIterations) && l_bReturn; i++)
    {
        //printf("Iteration %zu\n", i);

        IuP7proxy *l_iProxy = uP7createProxy(TM("/P7.Sink=Null /P7.Eto=0 /P7.Pool=1024"), i_pSessionFolder);

        if (l_iProxy)
        {
            IuP7Fifo **l_pFifo  = (IuP7Fifo**)malloc(i_szCpus * sizeof(IuP7Fifo*));

            if (l_pFifo)
            {

                for (uint8_t szI = 0; szI < i_szCpus; szI++)
                {
                    if (!l_iProxy->RegisterCpu(szI, false, GetPerformanceFrequency(), TM("x86 cpu test"), 512*1024, true, l_pFifo[szI]))
                    {
                        l_bReturn = false;
                        break;
                    }
                    if (l_iProxy->RegisterCpu(szI, false, GetPerformanceFrequency(), TM("x86 cpu test"), 512*1024, true, l_pFifo[szI]))
                    {
                        l_bReturn = false;
                        break;
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(50));

                for (uint8_t szI = 0; szI < i_szCpus; szI++)
                {
                    if (l_pFifo[szI])
                    {
                        l_iProxy->UnRegisterCpu((uint8_t)szI);
                        if (l_iProxy->UnRegisterCpu((uint8_t)szI))
                        {
                            l_bReturn = false;
                            break;
                        }
                        l_pFifo[szI]->Release();
                        l_pFifo[szI] = NULL;
                    }
                
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(50));

                free(l_pFifo);
            }
            else
            {
                l_bReturn = false;
            }


            if (l_iProxy)
            {
                l_iProxy->Release();
                l_iProxy = NULL;
            }
        }
        else
        {
            l_bReturn = false;
        }
    }

    return l_bReturn;
}

