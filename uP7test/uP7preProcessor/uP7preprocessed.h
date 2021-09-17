////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                     WARNING!                                                       //
//                                       this header is automatically generated                                       //
//                                                 DO NOT MODIFY IT                                                   //
//                                           Generated: 2021.09.17 18:39:47                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef UP7_TARGET_CPU_H
#define UP7_TARGET_CPU_H

uint32_t g_uSessionId = 3133061785;
uint8_t  g_bCrc7 = 101;


size_t g_szModules = 5;
struct stuP7Module g_pModules[] = 
{
    {"t2", euP7Level_Trace, 0, 121957187},
    {"t3", euP7Level_Trace, 1, 105179568},
    {"t4", euP7Level_Trace, 2, 222622901},
    {"t5", euP7Level_Trace, 3, 205845282},
    {"t6", euP7Level_Trace, 4, 189067663}
};

size_t g_szTelemetry = 5;
struct stuP7telemetry g_pTelemetry[] = 
{
    {"t2 Counter", 1916703671, true, 0},
    {"t3 Counter", 1032138516, true, 1},
    {"t4 Counter", 3389252769, true, 2},
    {"t5 Counter", 3526919718, true, 3},
    {"t6 Counter", 1803406187, true, 4}
};

static const struct stuP7arg g_pArgsId0[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_str_utf8, 3} };
static const struct stuP7arg g_pArgsId1[] = { {(uint8_t)euP7_arg_str_utf16, 0} };
static const struct stuP7arg g_pArgsId2[] = { {(uint8_t)euP7_arg_str_utf16, 3} };
static const struct stuP7arg g_pArgsId3[] = { {(uint8_t)euP7_arg_str_ansi, 0} };
static const struct stuP7arg g_pArgsId4[] = { {(uint8_t)euP7_arg_str_ansi, 3} };
static const struct stuP7arg g_pArgsId5[] = { {(uint8_t)euP7_arg_str_utf8, 0} };
static const struct stuP7arg g_pArgsId6[] = { {(uint8_t)euP7_arg_str_utf16, 0} };
static const struct stuP7arg g_pArgsId7[] = { {(uint8_t)euP7_arg_str_utf16, 3} };
static const struct stuP7arg g_pArgsId8[] = { {(uint8_t)euP7_arg_str_utf8, 0} };
static const struct stuP7arg g_pArgsId9[] = { {(uint8_t)euP7_arg_str_utf8, 3} };
static const struct stuP7arg g_pArgsId10[] = { {(uint8_t)euP7_arg_str_ansi, 0} };
static const struct stuP7arg g_pArgsId11[] = { {(uint8_t)euP7_arg_str_utf16, 0} };
static const struct stuP7arg g_pArgsId12[] = { {(uint8_t)euP7_arg_str_utf8, 0} };
static const struct stuP7arg g_pArgsId13[] = { {(uint8_t)euP7_arg_str_ansi, 0} };
static const struct stuP7arg g_pArgsId14[] = { {(uint8_t)euP7_arg_str_utf16, 0} };
static const struct stuP7arg g_pArgsId15[] = { {(uint8_t)euP7_arg_int8, 4} };
static const struct stuP7arg g_pArgsId16[] = { {(uint8_t)euP7_arg_int8, 4} };
static const struct stuP7arg g_pArgsId17[] = { {(uint8_t)euP7_arg_int8, 4} };
static const struct stuP7arg g_pArgsId18[] = { {(uint8_t)euP7_arg_int8, 4} };
static const struct stuP7arg g_pArgsId19[] = { {(uint8_t)euP7_arg_char16, 4} };
static const struct stuP7arg g_pArgsId20[] = { {(uint8_t)euP7_arg_char16, 4} };
static const struct stuP7arg g_pArgsId21[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId22[] = { {(uint8_t)euP7_arg_intmax, 8}, {(uint8_t)euP7_arg_intmax, 8}, {(uint8_t)euP7_arg_intmax, 8} };
static const struct stuP7arg g_pArgsId23[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_intmax, 8} };
static const struct stuP7arg g_pArgsId24[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_intmax, 8} };
static const struct stuP7arg g_pArgsId25[] = { {(uint8_t)euP7_arg_int64, 8}, {(uint8_t)euP7_arg_int64, 8}, {(uint8_t)euP7_arg_int64, 8} };
static const struct stuP7arg g_pArgsId26[] = { {(uint8_t)euP7_arg_int64, 8} };
static const struct stuP7arg g_pArgsId27[] = { {(uint8_t)euP7_arg_int64, 8}, {(uint8_t)euP7_arg_int64, 8} };
static const struct stuP7arg g_pArgsId28[] = { {(uint8_t)euP7_arg_int64, 8}, {(uint8_t)euP7_arg_int64, 8} };
static const struct stuP7arg g_pArgsId29[] = { {(uint8_t)euP7_arg_int64, 8} };
static const struct stuP7arg g_pArgsId30[] = { {(uint8_t)euP7_arg_int64, 8}, {(uint8_t)euP7_arg_int64, 8} };
static const struct stuP7arg g_pArgsId31[] = { {(uint8_t)euP7_arg_int16, 4}, {(uint8_t)euP7_arg_int16, 4}, {(uint8_t)euP7_arg_int16, 4} };
static const struct stuP7arg g_pArgsId32[] = { {(uint8_t)euP7_arg_int16, 4} };
static const struct stuP7arg g_pArgsId33[] = { {(uint8_t)euP7_arg_int16, 4}, {(uint8_t)euP7_arg_int16, 4} };
static const struct stuP7arg g_pArgsId34[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId35[] = { {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId36[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId37[] = { {(uint8_t)euP7_arg_int64, 8}, {(uint8_t)euP7_arg_int64, 8} };
static const struct stuP7arg g_pArgsId38[] = { {(uint8_t)euP7_arg_int64, 8} };
static const struct stuP7arg g_pArgsId39[] = { {(uint8_t)euP7_arg_int64, 8}, {(uint8_t)euP7_arg_int64, 8} };
static const struct stuP7arg g_pArgsId40[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId41[] = { {(uint8_t)euP7_arg_double, 8}, {(uint8_t)euP7_arg_double, 8}, {(uint8_t)euP7_arg_double, 8}, {(uint8_t)euP7_arg_double, 8}, {(uint8_t)euP7_arg_double, 8}, {(uint8_t)euP7_arg_double, 8}, {(uint8_t)euP7_arg_double, 8} };
static const struct stuP7arg g_pArgsId42[] = { {(uint8_t)euP7_arg_str_utf8, 0}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_str_utf8, 3}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int64, 8}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int64, 8}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_double, 8}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_intmax, 8}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int8, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int16, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_str_utf8, 0}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_double, 8}, {(uint8_t)euP7_arg_int64, 8} };
static const struct stuP7arg g_pArgsId43[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId44[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId45[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId46[] = { {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId47[] = { {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId55[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_str_utf8, 0} };

size_t g_szTraces = 50;
struct stuP7Trace g_pTraces[] = 
{
    {0, sizeof(g_pArgsId0)/sizeof(struct stuP7arg), g_pArgsId0},
    {1, sizeof(g_pArgsId1)/sizeof(struct stuP7arg), g_pArgsId1},
    {2, sizeof(g_pArgsId2)/sizeof(struct stuP7arg), g_pArgsId2},
    {3, sizeof(g_pArgsId3)/sizeof(struct stuP7arg), g_pArgsId3},
    {4, sizeof(g_pArgsId4)/sizeof(struct stuP7arg), g_pArgsId4},
    {5, sizeof(g_pArgsId5)/sizeof(struct stuP7arg), g_pArgsId5},
    {6, sizeof(g_pArgsId6)/sizeof(struct stuP7arg), g_pArgsId6},
    {7, sizeof(g_pArgsId7)/sizeof(struct stuP7arg), g_pArgsId7},
    {8, sizeof(g_pArgsId8)/sizeof(struct stuP7arg), g_pArgsId8},
    {9, sizeof(g_pArgsId9)/sizeof(struct stuP7arg), g_pArgsId9},
    {10, sizeof(g_pArgsId10)/sizeof(struct stuP7arg), g_pArgsId10},
    {11, sizeof(g_pArgsId11)/sizeof(struct stuP7arg), g_pArgsId11},
    {12, sizeof(g_pArgsId12)/sizeof(struct stuP7arg), g_pArgsId12},
    {13, sizeof(g_pArgsId13)/sizeof(struct stuP7arg), g_pArgsId13},
    {14, sizeof(g_pArgsId14)/sizeof(struct stuP7arg), g_pArgsId14},
    {15, sizeof(g_pArgsId15)/sizeof(struct stuP7arg), g_pArgsId15},
    {16, sizeof(g_pArgsId16)/sizeof(struct stuP7arg), g_pArgsId16},
    {17, sizeof(g_pArgsId17)/sizeof(struct stuP7arg), g_pArgsId17},
    {18, sizeof(g_pArgsId18)/sizeof(struct stuP7arg), g_pArgsId18},
    {19, sizeof(g_pArgsId19)/sizeof(struct stuP7arg), g_pArgsId19},
    {20, sizeof(g_pArgsId20)/sizeof(struct stuP7arg), g_pArgsId20},
    {21, sizeof(g_pArgsId21)/sizeof(struct stuP7arg), g_pArgsId21},
    {22, sizeof(g_pArgsId22)/sizeof(struct stuP7arg), g_pArgsId22},
    {23, sizeof(g_pArgsId23)/sizeof(struct stuP7arg), g_pArgsId23},
    {24, sizeof(g_pArgsId24)/sizeof(struct stuP7arg), g_pArgsId24},
    {25, sizeof(g_pArgsId25)/sizeof(struct stuP7arg), g_pArgsId25},
    {26, sizeof(g_pArgsId26)/sizeof(struct stuP7arg), g_pArgsId26},
    {27, sizeof(g_pArgsId27)/sizeof(struct stuP7arg), g_pArgsId27},
    {28, sizeof(g_pArgsId28)/sizeof(struct stuP7arg), g_pArgsId28},
    {29, sizeof(g_pArgsId29)/sizeof(struct stuP7arg), g_pArgsId29},
    {30, sizeof(g_pArgsId30)/sizeof(struct stuP7arg), g_pArgsId30},
    {31, sizeof(g_pArgsId31)/sizeof(struct stuP7arg), g_pArgsId31},
    {32, sizeof(g_pArgsId32)/sizeof(struct stuP7arg), g_pArgsId32},
    {33, sizeof(g_pArgsId33)/sizeof(struct stuP7arg), g_pArgsId33},
    {34, sizeof(g_pArgsId34)/sizeof(struct stuP7arg), g_pArgsId34},
    {35, sizeof(g_pArgsId35)/sizeof(struct stuP7arg), g_pArgsId35},
    {36, sizeof(g_pArgsId36)/sizeof(struct stuP7arg), g_pArgsId36},
    {37, sizeof(g_pArgsId37)/sizeof(struct stuP7arg), g_pArgsId37},
    {38, sizeof(g_pArgsId38)/sizeof(struct stuP7arg), g_pArgsId38},
    {39, sizeof(g_pArgsId39)/sizeof(struct stuP7arg), g_pArgsId39},
    {40, sizeof(g_pArgsId40)/sizeof(struct stuP7arg), g_pArgsId40},
    {41, sizeof(g_pArgsId41)/sizeof(struct stuP7arg), g_pArgsId41},
    {42, sizeof(g_pArgsId42)/sizeof(struct stuP7arg), g_pArgsId42},
    {43, sizeof(g_pArgsId43)/sizeof(struct stuP7arg), g_pArgsId43},
    {44, sizeof(g_pArgsId44)/sizeof(struct stuP7arg), g_pArgsId44},
    {45, sizeof(g_pArgsId45)/sizeof(struct stuP7arg), g_pArgsId45},
    {46, sizeof(g_pArgsId46)/sizeof(struct stuP7arg), g_pArgsId46},
    {47, sizeof(g_pArgsId47)/sizeof(struct stuP7arg), g_pArgsId47},
    {48, 0, NULL},
    {55, sizeof(g_pArgsId55)/sizeof(struct stuP7arg), g_pArgsId55}
};
//uint64_t g_uEpochTime = 0x1d7abda3eff56c0;
#endif //UP7_TARGET_CPU_H:25636362BD54E44C9EC09F71E33E890A90B7BDFA5B180F442F6383EF385A120E