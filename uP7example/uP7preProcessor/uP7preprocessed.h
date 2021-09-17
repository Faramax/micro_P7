////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                     WARNING!                                                       //
//                                       this header is automatically generated                                       //
//                                                 DO NOT MODIFY IT                                                   //
//                                           Generated: 2021.09.17 18:39:46                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef UP7_TARGET_CPU_H
#define UP7_TARGET_CPU_H

uint32_t g_uSessionId = 2268502963;
uint8_t  g_bCrc7 = 66;


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
//uint64_t g_uEpochTime = 0x1d7abda3e50aff8;
#endif //UP7_TARGET_CPU_H:AF92A9CC3A1FF1114F010B3E8DA28C14CFD9B30C7EF464905D2CF229A8A95087