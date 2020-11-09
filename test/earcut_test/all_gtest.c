#include "mylog.h"
#include "greatest.h"
#include "test_utils.h"
#include "polygon_earcut.h"

TEST signed_area_test(void) {
    PASS();
}

SUITE(unit_tests) {
    RUN_TEST(signed_area_test);
}

TEST ccw_test1(void) {
    // a counter-clockwise polygon
    coord_t xs[] = {70, 60, 0, 10};
    coord_t ys[] = {10, 60, 50, 0};
    const int n = ARR_LEN(xs);
    polygon_t polygon = allocate_polygon(n);
    for (int i=0; i<n; ++i) {
        coord_seq_setx(polygon, i, xs[i]);
        coord_seq_sety(polygon, i, ys[i]);
    }
    ASSERT_EQ(n, vertices_num(polygon));

    triangles_t *triangles = polygon_earcut(polygon, 0, NULL);
    ASSERT(NULL != triangles);
    DBG("m=%u", triangles->m);
    ASSERT_EQ(n-2, triangles->m);

    boxed_triangle expected_triangles[] = {
        {.tri = {2,3,0} },
        {.tri = {0,1,2} },
    };
    ASSERT_EQUAL_T(&expected_triangles, triangles->vidx, &boxed_triangle_type_info, NULL);

    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    free(triangles);
    free_polygon(polygon);
    PASS();
}

TEST cw_test2(void) {
    // a clockwise ploygon
    coord_t xs[] = {10,0,60,70};
    coord_t ys[] = {0,50,60,10};
    const int n = ARR_LEN(xs);
    polygon_t polygon = allocate_polygon(n);
    for (int i=0; i<n; ++i) {
        coord_seq_setx(polygon, i, xs[i]);
        coord_seq_sety(polygon, i, ys[i]);
    }
    ASSERT_EQ(n, vertices_num(polygon));

    triangles_t* triangles = polygon_earcut(polygon, 0, NULL);
    ASSERT(NULL != triangles);
    ASSERT_EQ(n-2, triangles->m);

    boxed_triangle expected_triangles[] = {
        {.tri = {1,0,3} },
        {.tri = {3,2,1} },
    };
    ASSERT_EQUAL_T(&expected_triangles, triangles->vidx, &boxed_triangle_type_info, NULL);

    //ASSERT( diff_areas(polygon, triangles) < 0.00001);

    free(triangles);
    free_polygon(polygon);
    PASS();
}

TEST cw_test3(void) {
    // clockwise
    coord_t xs[] = {3,5,12,9,5};
    coord_t ys[] = {4,11,8,5,6};
    const int n = ARR_LEN(xs);
    polygon_t polygon = allocate_polygon(n);
    for (int i=0; i<n; ++i) {
        coord_seq_setx(polygon, i, xs[i]);
        coord_seq_sety(polygon, i, ys[i]);
    }
    ASSERT_EQ(n, vertices_num(polygon));

    triangles_t* triangles = polygon_earcut(polygon, 0, NULL);
    ASSERT(NULL != triangles);
    ASSERT_EQ(n-2, triangles->m);
    boxed_triangle expected_triangles[] = {
        {.tri = {1,0,4} },
        {.tri = {4,3,2} },
        {.tri = {2,1,4} },
    };
    ASSERT_EQUAL_T(&expected_triangles, triangles->vidx, &boxed_triangle_type_info, NULL);

    //ASSERT( diff_areas(polygon, triangles) < 0.00001);

    free(triangles);
    free_polygon(polygon);
    PASS();
}

SUITE(earcut_tests) {
    RUN_TEST(ccw_test1);
    RUN_TEST(cw_test2);
    RUN_TEST(cw_test3);
}

TEST i18_test(void) {
#include "i18_data.h"
    const int n = ARR_LEN(x);
    polygon_t polygon = allocate_polygon(n);
    for (int i=0; i<n; ++i) {
        coord_seq_setx(polygon, i, x[i]);
        coord_seq_sety(polygon, i, y[i]);
    }
    ASSERT_EQ(n, vertices_num(polygon));

    triangles_t* triangles = polygon_earcut(polygon, 0, NULL);
    ASSERT(NULL != triangles);
    ASSERT_EQ(n-2, triangles->m);

    //print_triangles(triangles);
    ASSERT_EQUAL_T(expected_triangles, triangles->vidx, &boxed_triangle_type_info, NULL);
    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    PASS();
}

TEST comb_test(void) {
#include "comb_data.h"
    const int n = ARR_LEN(x);
    polygon_t polygon = allocate_polygon(n);
    for (int i=0; i<n; ++i) {
        coord_seq_setx(polygon, i, x[i]);
        coord_seq_sety(polygon, i, y[i]);
    }
    ASSERT_EQ(n, vertices_num(polygon));

    triangles_t* triangles = polygon_earcut(polygon, 0, NULL);
    ASSERT(NULL != triangles);
    ASSERT_EQ(n-2, triangles->m);

    //print_triangles(triangles);
    ASSERT_EQUAL_T(expected_triangles, triangles->vidx, &boxed_triangle_type_info, NULL);
    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    PASS();
}

TEST hand_test(void) {
#include "hand_data.h"
    const int n = ARR_LEN(x);
    polygon_t polygon = allocate_polygon(n);
    for (int i=0; i<n; ++i) {
        coord_seq_setx(polygon, i, x[i]);
        coord_seq_sety(polygon, i, y[i]);
    }
    ASSERT_EQ(n, vertices_num(polygon));

    triangles_t* triangles = polygon_earcut(polygon, 0, NULL);
    ASSERT(NULL != triangles);
    ASSERT_EQ(n-2, triangles->m);

vidx_t expected_triangles[] = {
    57, 58, 0, 0, 1, 2, 2, 3, 4, 4, 5, 6, 10, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 23, 24, 25, 25, 26, 27, 27, 28, 29, 30, 31, 32, 33, 34, 35, 35, 36, 37, 37, 38, 39, 39, 40, 41, 41, 42, 43, 45, 46, 47, 47, 48, 49, 50, 51, 52, 53, 54, 55, 55, 56, 57, 57, 0, 2, 2, 4, 6, 10, 12, 14, 14, 16, 18, 23, 25, 27, 27, 29, 30, 35, 37, 39, 39, 41, 43, 45, 47, 49, 49, 50, 52, 53, 55, 57, 2, 6, 7, 10, 14, 18, 23, 27, 30, 33, 35, 39, 44, 45, 49, 53, 57, 2, 9, 10, 18, 22, 23, 30, 33, 39, 43, 44, 49, 52, 53, 2, 7, 9, 18, 19, 22, 30, 32, 32, 33, 43, 43, 44, 52, 52, 53, 7, 9, 19, 20, 22, 32, 43, 52, 7, 8, 8, 9, 20, 22, 43, 52, 8, 20, 21, 21, 22, 52, 52, 8, 21
};

    //print_triangles(triangles);
    ASSERT_EQUAL_T(expected_triangles, triangles->vidx, &boxed_triangle_type_info, NULL);
    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    PASS();
}

SUITE(data_tests) {
    RUN_TEST(i18_test);
    RUN_TEST(comb_test);
    RUN_TEST(hand_test);
}

GREATEST_MAIN_DEFS();

int main(int argc, char* argv[]) {
    INFO("vidx_t.sz: %zu bytes, coord_t.sz: %zu bytes", sizeof(vidx_t), sizeof(coord_t));

    GREATEST_MAIN_BEGIN();

    RUN_SUITE(unit_tests);
    RUN_SUITE(earcut_tests);
    RUN_SUITE(data_tests);

    GREATEST_MAIN_END();
}

