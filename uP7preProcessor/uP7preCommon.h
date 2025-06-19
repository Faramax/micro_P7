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
#ifndef UP7_PRE_COMMON_H
#define UP7_PRE_COMMON_H

#define STRSAFE_NO_DEPRECATE

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <random>
#include <inttypes.h>

#include "GTypes.h"
#include "Length.h"
#include "AList.h"
#include "RBTree.h"
#include "PString.h"
#include "UTF.h"
#include "WString.h"
#include "PAtomic.h"
#include "PFileSystem.h"
#include "PTime.h"
#include "PProcess.h"
#include "PLock.h"
#include "Lock.h"
#include "PSignal.h"
#include "IMEvent.h"
#include "PMEvent.h"
#include "PThreadShell.h"
#include "PSystem.h"
#include "PShared.h"
#include "Ticks.h"
#include "IFile.h"
#include "PFile.h"
#include "PSocket.h"
#include "P7_Version.h"
#include "P7_Client.h"
#include "P7_Trace.h"
#include "P7_Telemetry.h"
#include "P7_Extensions.h"
#include "INode.h"
#include "Keccak.h"

#pragma warning(disable: 4996)

#include "uP7proxyApi.h"
#include "uP7preConfig.h"
#include "uP7preDefines.h"
#include "uP7preTools.h"
#include "uP7.h"
#include "uP7hash.h"
#include "uP7preFunction.h"
#include "uP7preManager.h"
#include "uP7preFile.h"
#include "uP7sessionFile.h"
#include <string>
#include <list>

#if !defined(MAXUINT16)
    #define MAXUINT16   ((tUINT16)~((tUINT16)0u))
#endif

#endif //UP7_PRE_COMMON_H