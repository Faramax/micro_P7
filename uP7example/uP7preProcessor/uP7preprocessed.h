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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                     WARNING!                                                       //
//                                       this header is automatically generated                                       //
//                                                 DO NOT MODIFY IT                                                   //
//                                           Generated: 2021.08.13 19:43:58                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef UP7_TARGET_CPU_H
#define UP7_TARGET_CPU_H

uint32_t g_uSessionId = 1068459680;
uint8_t  g_bCrc7 = 81;


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

static const struct stuP7arg g_pArgsId1[] = { {(uint8_t)euP7_arg_int64, 8} };

size_t g_szTraces = 4;
struct stuP7Trace g_pTraces[] = 
{
    {0, 0, NULL},
    {1, sizeof(g_pArgsId1)/sizeof(struct stuP7arg), g_pArgsId1},
    {2, 0, NULL},
    {3, 0, NULL}
};
//uint64_t g_uEpochTime = 0x1d790732d520df0;
#endif //UP7_TARGET_CPU_H:E692E78BB2CEACD19917584D2ADFC66CE70116565C0F78C3628F98241DC2B69D