////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                     WARNING!                                                       //
//                                       this header was automatically generated                                      //
//                                                 DO NOT MODIFY IT                                                   //
//                                   WE DO NOT RECOMMEND TO COMMIT IT (svn, hq, etc.)                                 //
//                                  >>>INSTEAD PLEASE COMMIT XML CONFIGURATION FILE<<<                                //
//                                           Generated: 2024.09.28 18:53:06                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef UP7_TARGET_CPU_H
#define UP7_TARGET_CPU_H

#define UP7_CONSECUTIVE_TRACE_ID

uint32_t g_uSessionId = 2594892637;
uint8_t  g_bCrc7 = 8;


size_t g_szModules = 21;
struct stuP7Module g_pModules[] = 
{
    {"TestA0/TestB0/TestC00<", euP7Level_Trace, 0, 1229443239},
    {"TestA0/TestB0/TestC01<", euP7Level_Trace, 1, 1262851382},
    {"TestA0/TestB0/TestC02<", euP7Level_Trace, 2, 3443691589},
    {"TestA0/TestB0/TestC03<", euP7Level_Trace, 3, 3342878780},
    {"TestA0/TestB1/TestC00<", euP7Level_Trace, 4, 559809490},
    {"TestA0/TestB1/TestC01<", euP7Level_Trace, 5, 526401347},
    {"TestA0/TestB1/TestC02<", euP7Level_Trace, 6, 2639836888},
    {"TestA0/TestB1/TestC03<", euP7Level_Trace, 7, 593217633},
    {"TestA1/TestB0/TestC00<", euP7Level_Trace, 8, 2777699360},
    {"TestA1/TestB0/TestC01<", euP7Level_Trace, 9, 2878512169},
    {"TestA1/TestB0/TestC02<", euP7Level_Trace, 10, 697671962},
    {"TestA1/TestB0/TestC03<", euP7Level_Trace, 11, 664263819},
    {"TestA1/TestB1/TestC00<", euP7Level_Trace, 12, 1280212797},
    {"TestA1/TestB1/TestC01<", euP7Level_Trace, 13, 1179399988},
    {"TestA1/TestB1/TestC02<", euP7Level_Trace, 14, 1213396511},
    {"TestA1/TestB1/TestC03<", euP7Level_Trace, 15, 1246804654},
    {"t2", euP7Level_Trace, 16, 121957187},
    {"t3", euP7Level_Trace, 17, 105179568},
    {"t4", euP7Level_Trace, 18, 222622901},
    {"t5", euP7Level_Trace, 19, 205845282},
    {"t6", euP7Level_Trace, 20, 189067663}
};

size_t g_szTelemetry = 15;
struct stuP7telemetry g_pTelemetry[] = 
{
    {"Tel01<", 841880019, true, 0},
    {"Tel02<", 807883496, true, 1},
    {"Tel03<", 908696305, true, 2},
    {"Tel04<", 3022131846, true, 3},
    {"Tel05<", 2988723703, true, 4},
    {"Tel06<", 807191948, true, 5},
    {"Tel07<", 3055539989, true, 6},
    {"Tel08<", 874008234, true, 7},
    {"Tel09<", 840600091, true, 8},
    {"Tel10<", 578042621, true, 9},
    {"t2 Counter", 1916703671, true, 10},
    {"t3 Counter", 1032138516, true, 11},
    {"t4 Counter", 3389252769, true, 12},
    {"t5 Counter", 3526919718, true, 13},
    {"t6 Counter", 1803406187, true, 14}
};

static const struct stuP7arg g_pArgsId0[] = { {(uint8_t)euP7_arg_str_utf32, 3} };
static const struct stuP7arg g_pArgsId1[] = { {(uint8_t)euP7_arg_str_utf8, 0} };
static const struct stuP7arg g_pArgsId2[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_str_utf8, 3} };
static const struct stuP7arg g_pArgsId3[] = { {(uint8_t)euP7_arg_str_utf32, 0} };
static const struct stuP7arg g_pArgsId4[] = { {(uint8_t)euP7_arg_str_ansi, 0} };
static const struct stuP7arg g_pArgsId5[] = { {(uint8_t)euP7_arg_str_ansi, 3} };
static const struct stuP7arg g_pArgsId6[] = { {(uint8_t)euP7_arg_str_utf32, 0} };
static const struct stuP7arg g_pArgsId7[] = { {(uint8_t)euP7_arg_str_utf32, 3} };
static const struct stuP7arg g_pArgsId8[] = { {(uint8_t)euP7_arg_str_utf8, 0} };
static const struct stuP7arg g_pArgsId9[] = { {(uint8_t)euP7_arg_str_utf8, 3} };
static const struct stuP7arg g_pArgsId10[] = { {(uint8_t)euP7_arg_str_ansi, 0} };
static const struct stuP7arg g_pArgsId11[] = { {(uint8_t)euP7_arg_str_utf32, 0} };
static const struct stuP7arg g_pArgsId12[] = { {(uint8_t)euP7_arg_str_utf8, 0} };
static const struct stuP7arg g_pArgsId13[] = { {(uint8_t)euP7_arg_str_ansi, 0} };
static const struct stuP7arg g_pArgsId14[] = { {(uint8_t)euP7_arg_str_utf32, 0} };
static const struct stuP7arg g_pArgsId15[] = { {(uint8_t)euP7_arg_int8, 4} };
static const struct stuP7arg g_pArgsId16[] = { {(uint8_t)euP7_arg_int8, 4} };
static const struct stuP7arg g_pArgsId17[] = { {(uint8_t)euP7_arg_int8, 4} };
static const struct stuP7arg g_pArgsId18[] = { {(uint8_t)euP7_arg_int8, 4} };
static const struct stuP7arg g_pArgsId19[] = { {(uint8_t)euP7_arg_char32, 4} };
static const struct stuP7arg g_pArgsId20[] = { {(uint8_t)euP7_arg_char32, 4} };
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
static const struct stuP7arg g_pArgsId34[] = { {(uint8_t)euP7_arg_int64, 8}, {(uint8_t)euP7_arg_int64, 8} };
static const struct stuP7arg g_pArgsId35[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId36[] = { {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId37[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId38[] = { {(uint8_t)euP7_arg_int64, 8}, {(uint8_t)euP7_arg_int64, 8} };
static const struct stuP7arg g_pArgsId39[] = { {(uint8_t)euP7_arg_int64, 8} };
static const struct stuP7arg g_pArgsId40[] = { {(uint8_t)euP7_arg_int64, 8}, {(uint8_t)euP7_arg_int64, 8} };
static const struct stuP7arg g_pArgsId41[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId42[] = { {(uint8_t)euP7_arg_double, 8}, {(uint8_t)euP7_arg_double, 8}, {(uint8_t)euP7_arg_double, 8}, {(uint8_t)euP7_arg_double, 8}, {(uint8_t)euP7_arg_double, 8}, {(uint8_t)euP7_arg_double, 8}, {(uint8_t)euP7_arg_double, 8} };
static const struct stuP7arg g_pArgsId43[] = { {(uint8_t)euP7_arg_str_utf8, 0}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_str_utf8, 3}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int64, 8}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int64, 8}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_double, 8}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_intmax, 8}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int8, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int16, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_str_utf8, 0}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_double, 8}, {(uint8_t)euP7_arg_int64, 8} };
static const struct stuP7arg g_pArgsId44[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId45[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId46[] = { {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4}, {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId47[] = { {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId48[] = { {(uint8_t)euP7_arg_int32, 4} };
static const struct stuP7arg g_pArgsId50[] = { {(uint8_t)euP7_arg_int64, 8}, {(uint8_t)euP7_arg_str_utf8, 0} };

size_t g_szTraces = 51;
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
    {48, sizeof(g_pArgsId48)/sizeof(struct stuP7arg), g_pArgsId48},
    {49, 0, NULL},
    {50, sizeof(g_pArgsId50)/sizeof(struct stuP7arg), g_pArgsId50}
};
#endif