////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                     WARNING!                                                       //
//                                       this header is automatically generated                                       //
//                                                 DO NOT MODIFY IT                                                   //
//                                           Generated: 2026.07.07 15:34:31                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef UP7_TARGET_CPU_H
#define UP7_TARGET_CPU_H

uint32_t g_uSessionId = 3324366369;
uint8_t  g_bCrc7 = 7;


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
//uint64_t g_uEpochTime = 0x1dd0e15572f5946;
#endif //UP7_TARGET_CPU_H:CD5EDA06B8818C384B39934B2BB6DEC963DC45F8B13EFEEFD52A98EA3A66F8D3