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
#ifndef ISIGNAL_H
#define ISIGNAL_H

enum eCrashCode
{
    eCrashException,
    eCrashPureCall,
    eCrashMemAlloc,
    eCrashInvalidParameter,
    eCrashSignal
};

typedef void (__cdecl *fnCrashHandler)(eCrashCode i_eCode, const void *i_pCrashContext, void *i_pUserContext);

struct stChContext
{
    volatile int      iInstalled;
    volatile int      iProcessed;
    void             *pUserContext;
    fnCrashHandler    pUserHandler;
};

#endif //ISIGNAL_H