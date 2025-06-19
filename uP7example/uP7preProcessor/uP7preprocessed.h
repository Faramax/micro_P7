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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                     WARNING!                                                       //
//                                       this header was automatically generated                                      //
//                                                 DO NOT MODIFY IT                                                   //
//                                   WE DO NOT RECOMMEND TO COMMIT IT (svn, hq, etc.)                                 //
//                                  >>>INSTEAD PLEASE COMMIT XML CONFIGURATION FILE<<<                                //
//                                           Generated: 2024.09.28 18:53:08                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef UP7_TARGET_CPU_H
#define UP7_TARGET_CPU_H

#define UP7_CONSECUTIVE_TRACE_ID

uint32_t g_uSessionId = 3509174532;
uint8_t  g_bCrc7 = 20;


size_t g_szModules = 2;
struct stuP7Module g_pModules[] = 
{
    {"Module", euP7Level_Trace, 0, 3304874173},
    {"XML Module", euP7Level_Trace, 1, 234093310}
};

size_t g_szTelemetry = 2;
struct stuP7telemetry g_pTelemetry[] = 
{
    {"CPU/Cycle", 3385683884, true, 0},
    {"XML Counter", 622366530, true, 1}
};

static const struct stuP7arg g_pArgsId2[] = { {(uint8_t)euP7_arg_int64, 8} };

size_t g_szTraces = 4;
struct stuP7Trace g_pTraces[] = 
{
    {0, 0, NULL},
    {1, 0, NULL},
    {2, sizeof(g_pArgsId2)/sizeof(struct stuP7arg), g_pArgsId2},
    {3, 0, NULL}
};
#endif