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
// 2012-2023 (c) Baical                                                                                                /
//                                                                                                                     /
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                     WARNING!                                                       //
//                                       this header is automatically generated                                       //
//                                                 DO NOT MODIFY IT                                                   //
//                                           Generated: 2021.12.25 18:48:46                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef UP7_TARGET_CPU_H
#define UP7_TARGET_CPU_H

#define UP7_CONSECUTIVE_TRACE_ID

uint32_t g_uSessionId = 3681427172;
uint8_t  g_bCrc7 = 112;


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
//uint64_t g_uEpochTime = 0x1d7f9b7aa69c9b0;
#endif //UP7_TARGET_CPU_H:08880F4733587735FE4BC1C1702C2BF23C0902F3D07B72BA6E8BE4BF1A127EC9