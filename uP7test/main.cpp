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
#include "uP7preprocessed.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32) || defined(_WIN64)
int wmain(int i_iArgC, tXCHAR *i_pArgV[])
#elif defined(__linux__)
int main(int i_iArgC, tXCHAR *i_pArgV[])
#endif
{
    if (i_iArgC < 2)
    {
        printf("Error: please provide path to <uP7>/uP7Test/uP7preProcessor\n");
        printf("Example: uP7Test ~/Projects/uP7/uP7Test/uP7preProcessor\n");
        return -1;
    }

    CWString          l_cDir(i_pArgV[1]);
    CBList<CWString*> l_cuP7files;
    CFSYS::Enumerate_Files(&l_cuP7files, &l_cDir, TM("*.") uP7_FILES_EXT);

    if (!l_cuP7files.Count())
    {
        printf("Error: Provided dir is empty, no session files found\n");
        printf("Please use preprocessor to generate\n");
        return -1;
    }

    l_cuP7files.Clear(TRUE);

    printf("Execute t1CreateDestroy ... \n");
    if (t1CreateDestroy(32, 255, i_pArgV[1]))
    {
        printf("Succeed!\n");
    }
    else
    {
        printf("Failed!\n");
        return -1;
    }
    
    
    printf("Execute t2Format ... \n");
    if (t2Format(i_pArgV[1]))
    {
        printf("Succeed!\n");
    }
    else
    {
        printf("Failed!\n");
        return -1;
    }
    
    printf("Execute t3Performance ... \n");
    if (t3Performance(i_pArgV[1]))
    {
        printf("Succeed!\n");
    }
    else
    {
        printf("Failed!\n");
        return -1;
    }
    
    printf("Execute t4MultiCore ... \n");
    if (t4MultiCore(255, i_pArgV[1]))
    {
        printf("Succeed!\n");
    }
    else
    {
        printf("Failed!\n");
        return -1;
    }
    
    printf("Execute t5Time ... \n");
    if (t5Time(i_pArgV[1]))
    {
        printf("Succeed!\n");
    }
    else
    {
        printf("Failed!\n");
        return -1;
    }
    
    printf("Execute t6Resync ... \n");
    if (t6Resync(i_pArgV[1]))
    {
        printf("Succeed!\n");
    }
    else
    {
        printf("Failed!\n");
        return -1;
    }


    return 0;
}
