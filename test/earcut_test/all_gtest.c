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
    vertices_t vertices = vertices_create(n, xs, ys);
    ASSERT_EQ(n, vertices_num(vertices));

    triangles_t triangles = polygon_earcut(vertices, NULL);
    ASSERT(NULL != triangles);
    __auto_type m = triangles_num(triangles);
    ASSERT_EQ(n-2, m);

    boxed_triangle expected_triangles[] = {
        {.tri = {2,3,0} },
        {.tri = {0,1,2} },
    };
    vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(&expected_triangles, tri, &boxed_triangle_type_info, NULL);

    polygon_t polygon = polygon_build(vertices, NULL);
    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    polygon_destroy(polygon);
    triangles_free(triangles);

    PASS();
}

TEST cw_test2(void) {
    // a clockwise ploygon
    coord_t xs[] = {10,0,60,70};
    coord_t ys[] = {0,50,60,10};
    const int n = ARR_LEN(xs);
    vertices_t vertices = vertices_create(n, xs, ys);
    ASSERT_EQ(n, vertices_num(vertices));

    triangles_t triangles = polygon_earcut(vertices, NULL);
    ASSERT(NULL != triangles);
    __auto_type m = triangles_num(triangles);
    ASSERT_EQ(n-2, m);

    boxed_triangle expected_triangles[] = {
        {.tri = {1,0,3} },
        {.tri = {3,2,1} },
    };
    vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(&expected_triangles, tri, &boxed_triangle_type_info, NULL);

    polygon_t polygon = polygon_build(vertices, NULL);
    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    triangles_free(triangles);
    polygon_destroy(polygon);

    PASS();
}

TEST cw_test3(void) {
    // clockwise
    coord_t xs[] = {3,5,12,9,5};
    coord_t ys[] = {4,11,8,5,6};
    const int n = ARR_LEN(xs);
    vertices_t vertices = vertices_create(n, xs, ys);
    ASSERT_EQ(n, vertices_num(vertices));

    triangles_t triangles = polygon_earcut(vertices, NULL);
    ASSERT(NULL != triangles);
    __auto_type m = triangles_num(triangles);
    ASSERT_EQ(n-2, m);
    boxed_triangle expected_triangles[] = {
        {.tri = {1,0,4} },
        {.tri = {4,3,2} },
        {.tri = {2,1,4} },
    };
    vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(&expected_triangles, tri, &boxed_triangle_type_info, NULL);

    polygon_t polygon = polygon_build(vertices, NULL);
    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    triangles_free(triangles);
    polygon_destroy(polygon);

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
    vertices_t vertices = vertices_create(n, x, y);
    ASSERT_EQ(n, vertices_num(vertices));

    triangles_t triangles = polygon_earcut(vertices, NULL);
    ASSERT(NULL != triangles);
    __auto_type m = triangles_num(triangles);
    ASSERT_EQ(n-2, m);

    //print_triangles(triangles);
    vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(expected_triangles, tri, &boxed_triangle_type_info, NULL);
    polygon_t polygon = polygon_build(vertices, NULL);
    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    triangles_free(triangles);
    polygon_destroy(polygon);

    PASS();
}

TEST comb_test(void) {
#include "comb_data.h"
    const int n = ARR_LEN(x);
    vertices_t vertices = vertices_create(n, x, y);
    ASSERT_EQ(n, vertices_num(vertices));

    triangles_t triangles = polygon_earcut(vertices, NULL);
    ASSERT(NULL != triangles);
    __auto_type m = triangles_num(triangles);
    ASSERT_EQ(n-2, m);

    //print_triangles(triangles);
    vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(expected_triangles, tri, &boxed_triangle_type_info, NULL);
    polygon_t polygon = polygon_build(vertices, NULL);
    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    triangles_free(triangles);
    polygon_destroy(polygon);

    PASS();
}

TEST hand_test(void) {
#include "hand_data.h"
    const int n = ARR_LEN(x);
    vertices_t vertices = vertices_create(n, x, y);
    ASSERT_EQ(n, vertices_num(vertices));

    triangles_t triangles = polygon_earcut(vertices, NULL);
    ASSERT(NULL != triangles);
    __auto_type m = triangles_num(triangles);
    ASSERT_EQ(n-2, m);

vidx_t expected_triangles[] = {
    57, 58, 0, 0, 1, 2, 2, 3, 4, 4, 5, 6, 10, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 23, 24, 25, 25, 26, 27, 27, 28, 29, 30, 31, 32, 33, 34, 35, 35, 36, 37, 37, 38, 39, 39, 40, 41, 41, 42, 43, 45, 46, 47, 47, 48, 49, 50, 51, 52, 53, 54, 55, 55, 56, 57, 57, 0, 2, 2, 4, 6, 10, 12, 14, 14, 16, 18, 23, 25, 27, 27, 29, 30, 35, 37, 39, 39, 41, 43, 45, 47, 49, 49, 50, 52, 53, 55, 57, 2, 6, 7, 10, 14, 18, 23, 27, 30, 33, 35, 39, 44, 45, 49, 53, 57, 2, 9, 10, 18, 22, 23, 30, 33, 39, 43, 44, 49, 52, 53, 2, 7, 9, 18, 19, 22, 30, 32, 32, 33, 43, 43, 44, 52, 52, 53, 7, 9, 19, 20, 22, 32, 43, 52, 7, 8, 8, 9, 20, 22, 43, 52, 8, 20, 21, 21, 22, 52, 52, 8, 21
};

    //print_triangles(triangles);
    vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(expected_triangles, tri, &boxed_triangle_type_info, NULL);
    polygon_t polygon = polygon_build(vertices, NULL);
    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    triangles_free(triangles);
    polygon_destroy(polygon);

    PASS();
}

SUITE(data_tests) {
    RUN_TEST(i18_test);
    RUN_TEST(comb_test);
    RUN_TEST(hand_test);
}

TEST hole1_test(void) {
    // counter-clockwise
    coord_t x[] = {0, 100, 100,   0, 20, 80, 80, 20};
    coord_t y[] = {0,   0, 100, 100, 20, 20, 80, 80};
    vidx_t holeIndices[] = {4};
    holes_t holes = holes_create(ARR_LEN(holeIndices), holeIndices);

    const int n = ARR_LEN(x);
    vertices_t vertices = vertices_create(n, x, y);
    ASSERT_EQ(n, vertices_num(vertices));

    triangles_t triangles = polygon_earcut(vertices, holes);
    ASSERT(NULL != triangles);
    __auto_type m = triangles_num(triangles);
    int expected_triangle_num = n + ARR_LEN(holeIndices) * 2 - 2;
    ASSERT_EQ(expected_triangle_num, m); // 

    vidx_t expected_triangles[] = {
        3,0,4, 5,4,0, 3,4,7, 5,0,1, 2,3,7, 6,5,1, 2,7,6, 6,1,2
    };

    vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(expected_triangles, tri, &boxed_triangle_type_info, NULL);

    polygon_t polygon = polygon_build(vertices, holes);
    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    triangles_free(triangles);
    polygon_destroy(polygon);

    PASS();
}

SUITE(hole_tests) {
    RUN_TEST(hole1_test);
}

GREATEST_MAIN_DEFS();

int main(int argc, char* argv[]) {
    INFO("vidx_t.sz: %zu bytes, coord_t.sz: %zu bytes", sizeof(vidx_t), sizeof(coord_t));

    GREATEST_MAIN_BEGIN();

    RUN_SUITE(unit_tests);
    RUN_SUITE(earcut_tests);
    RUN_SUITE(data_tests);
    RUN_SUITE(hole_tests);

    GREATEST_MAIN_END();
}

