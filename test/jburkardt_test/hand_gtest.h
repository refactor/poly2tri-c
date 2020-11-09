#include "mylog.h"

#include "polygon_triangulate.h"

#include "greatest.h"
#include "test_utils.h"

vidx_t expected_triangles[][3] = {
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

TEST hand_test(void) {
    #include "hand_data.h"
    printf("tri.len: %zu\n", ARR_LEN(expected_triangles));
    printf("tri.sz: %zu\n", sizeof(expected_triangles));
    const int n = ARR_LEN(x);
    ASSERT_EQ(n, ARR_LEN(y));
    DBG("arr.len: %u", n);
    polygon_t* polygon = allocate_polygon(n);
    for (__auto_type i=0; i<polygon->n; ++i) {
        coord_seq_setx(polygon, i, x[i]);
        coord_seq_sety(polygon, i, y[i]);
    }

    triangles_t* triangles = polygon_triangulate(polygon);

    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    const int tri_num = n - 2;
//    i4mat_transpose_print(3, tri_num, (int*)triangles, "Gotcha hand Triangles");

    boxed_triangle* etris = (boxed_triangle*)expected_triangles;
    sort_triangles(tri_num, etris);
    boxed_triangle* btris = (boxed_triangle*)triangles->vidx;
    sort_triangles(tri_num, btris);
    for (int i = 0; i < tri_num; ++i) 
        ASSERT_EQUAL_T(&etris[i], &btris[i], &boxed_triangle_type_info, NULL);

    free(triangles);
    PASS();
}

SUITE(the_hand_suite) {
    RUN_TEST(hand_test);
}

