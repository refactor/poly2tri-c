double x[] = {
    0.496200,
    0.557100,
    0.606100,
    0.644900,
    0.627100,
    0.593200,
    0.568900,
    0.539800,
    0.500500,
    0.486000,
    0.481700,
    0.493500,
    0.495700,
    0.496800,
    0.489800,
    0.477900,
    0.461800,
    0.457500,
    0.456900,
    0.456400,
    0.448300,
    0.437500,
    0.418100,
    0.405200,
    0.396100,
    0.381500,
    0.369100,
    0.351400,
    0.329800,
    0.334700,
    0.348700,
    0.354600,
    0.372400,
    0.360500,
    0.339000,
    0.314200,
    0.279800,
    0.272200,
    0.256100,
    0.249600,
    0.256100,
    0.287300,
    0.297000,
    0.322800,
    0.312600,
    0.288400,
    0.251800,
    0.231300,
    0.215100,
    0.213000,
    0.235100,
    0.250700,
    0.289400,
    0.299100,
    0.306100,
    0.313100,
    0.323900,
    0.336300,
    0.356200,
};
double y[] = {
    0.208600,
    0.319900,
    0.415800,
    0.500900,
    0.534400,
    0.512700,
    0.469200,
    0.424000,
    0.433900,
    0.501800,
    0.569700,
    0.663800,
    0.733500,
    0.783300,
    0.840300,
    0.855700,
    0.850200,
    0.822200,
    0.770600,
    0.736200,
    0.678300,
    0.612200,
    0.595000,
    0.642100,
    0.714500,
    0.773300,
    0.862000,
    0.885500,
    0.866500,
    0.817600,
    0.744300,
    0.688200,
    0.586000,
    0.586000,
    0.641200,
    0.701800,
    0.811300,
    0.815800,
    0.808600,
    0.786900,
    0.754300,
    0.656600,
    0.622200,
    0.537100,
    0.530800,
    0.557900,
    0.626700,
    0.657500,
    0.656600,
    0.621300,
    0.572400,
    0.535300,
    0.461100,
    0.420400,
    0.363300,
    0.319000,
    0.264700,
    0.235700,
    0.217600,
};

int expected_triangles[][3] = {
    { 1, 58,  0},  
    { 2, 58,  1},  
    { 3, 58,  2},  
    { 5,  3,  4},   
    { 6,  3,  5},   
    { 7,  3,  6},   
    {12, 10, 11},
    {13, 10, 12},
    {14, 10, 13},
    {15, 10, 14},
    {16, 10, 15},
    {17, 10, 16},
    {18, 10, 17},
    {19, 10, 18},
    {20, 10, 19},
    {21, 10, 20},
    {22, 10, 21},
    {25, 23, 24},
    {27, 25, 26},
    {28, 25, 27},
    {29, 25, 28},
    {30, 25, 29},
    {31, 25, 30},
    {32, 25, 31},
    {35, 33, 34},
    {37, 35, 36},
    {38, 35, 37},
    {39, 35, 38},
    {40, 35, 39},
    {41, 35, 40},
    {42, 35, 41},
    {43, 35, 42},
    {47, 45, 46},
    {48, 45, 47},
    {49, 45, 48},
    {50, 45, 49},
    {51, 45, 50},
    {52, 45, 51},
    {53, 45, 52},
    {54, 45, 53},
    {55, 45, 54},
    {56, 45, 55},
    {57, 45, 56},
    {58, 45, 57},
    { 7, 58,  3},  
    { 8, 58,  7},  
    { 9, 58,  8},  
    {10, 58,  9}, 
    {22, 58, 10},
    {23, 58, 22},
    {25, 58, 23},
    {32, 58, 25},
    {33, 58, 32},
    {35, 58, 33},
    {43, 58, 35},
    {44, 58, 43},
    {45, 58, 44},
};

