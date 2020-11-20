#include "mylog.h"
#include "greatest.h"
#include "test_utils.h"
#include "data_utils.h"
#include "polygon_earcut.h"
#include <math.h>
#include <assert.h>

static vertices_t rotate_vertices(const int n, const coord_t xs[n], const coord_t ys[n], double angle) {
    assert(n > 0);
    double csita = cos(angle);
    double ssita = sin(angle);

    coord_t* seq_x = calloc(n, sizeof(seq_x[0]));
    coord_t* seq_y = calloc(n, sizeof(seq_y[0]));
    for (int i = 0; i < n; ++i) {
        seq_x[i] = csita * xs[i] - ssita * ys[i];
        seq_y[i] = ssita * xs[i] + csita * ys[i];
    }
    vertices_t vertices = vertices_create(n, seq_x, seq_y);
    free(seq_x); free(seq_y);
    return vertices;
}

TEST hole_rotate_test(const int n, const coord_t xs[n], const coord_t ys[n], double angle, const vidx_t hn, const vidx_t holeIndices[hn]) {
    const vertices_t vertices = rotate_vertices(n, xs, ys, angle);
    ASSERT_EQ(n, vertices_num(vertices));

    const holes_t holes = holes_create(hn, holeIndices);
    const triangles_t triangles = polygon_earcut(vertices, holes);
    ASSERT(NULL != triangles);
    vidx_t m = triangles_num(triangles);
    ASSERT_EQ_FMT(n + 2*holes_num(holes) - 2, m, "%d");

    const polygon_t polygon = polygon_build(vertices, holes);
    ASSERT(diff_areas(polygon, triangles) < 1e-6);

    polygon_destroy(polygon);
    triangles_free(triangles);

    PASS();
}

TEST rotate_test(const boxed_triangle* expected_triangles, const int n, const coord_t xs[n], const coord_t ys[n], double angle) {
    const vertices_t vertices = rotate_vertices(n, xs, ys, angle);
    ASSERT_EQ(n, vertices_num(vertices));
    DBG("vnum: %d", n);

    const triangles_t triangles = polygon_earcut(vertices, NULL);
    ASSERT(NULL != triangles);
    __auto_type m = triangles_num(triangles);
    ASSERT_EQ(n-2, m);

    vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(expected_triangles, tri, &boxed_triangle_type_info, NULL);

    const polygon_t polygon = polygon_build(vertices, NULL);
//    char msg[128] = {0};
//    snprintf(msg, sizeof(msg), "angle=%g", angle * 180 / M_PI);
//    DBG("rotate: %s", msg);
    ASSERT(diff_areas(polygon, triangles) < 1e-6);

    polygon_destroy(polygon);
    triangles_free(triangles);

    PASS();
}

SUITE(cw_quadrangle_tests) {
    // a clockwise ploygon
    coord_t xs[] = {10,0,60,70};
    coord_t ys[] = {0,50,60,10};
    const int n = ARR_LEN(xs);

    boxed_triangle expected_triangles[] = {
        {.tri = {1,0,3} },
        {.tri = {3,2,1} },
    };
    for (int i = 1; i <=11; ++i) {
        RUN_TESTp(rotate_test, expected_triangles, n, xs, ys, 2*M_PI/i);
    }
}

SUITE(pentagon_tests) {
    // clockwise
    coord_t xs[] = {3,5,12,9,5};
    coord_t ys[] = {4,11,8,5,6};
    const int n = ARR_LEN(xs);
    boxed_triangle expected_triangles[] = {
        {.tri = {1,0,4} },
        {.tri = {4,3,2} },
        {.tri = {2,1,4} },
    };
    for (int i = 1; i <=9; ++i) {
        RUN_TESTp(rotate_test, expected_triangles, n, xs, ys, 2*M_PI/i);
    }
}

SUITE(ccw_quadrangle_tests) {
    //*
    // a counter-clockwise quadrangle
    coord_t xs[] = {70, 60, 0, 10};
    coord_t ys[] = {10, 60, 50, 0};
    const int n = ARR_LEN(xs);
    boxed_triangle expected_triangles[] = {
        {.tri = {2,3,0} },
        {.tri = {0,1,2} },
    };
    //*/
    for (int i = 1; i <= 7; ++i) {
        greatest_set_test_suffix("ccw_quadrangle");
        RUN_TESTp(rotate_test, expected_triangles, n, xs, ys, 2*M_PI/i);
    }
}

SUITE(i18_tests) {
#include "i18_data.h"
    const vidx_t n = ARR_LEN(x);
    for (int i = 1; i <=19; ++i) {
        greatest_set_test_suffix("i18");
        RUN_TESTp(rotate_test, (boxed_triangle*)expected_triangles, n, x, y, 2*M_PI/i);
    }
}

SUITE(comb_tests) {
#include "comb_data.h"
    const vidx_t n = ARR_LEN(x);
    for (int i = 1; i <=19; ++i) {
        greatest_set_test_suffix("hand");
        RUN_TESTp(rotate_test, (boxed_triangle*)expected_triangles, n, x, y, 2*M_PI/i);
    }
}

SUITE(hand_tests) {
#include "hand_data.h"
    const int n = ARR_LEN(x);
    /*
    boxed_triangle expected_triangles[] = {
        {57, 58, 0}, {0, 1, 2}, {2, 3, 4}, {4, 5, 6}, {10, 11, 12}, {12, 13, 14}, {14, 15, 16}, {16, 17, 18}, {23, 24, 25}, 
        {25, 26, 27}, {27, 28, 29}, {30, 31, 32}, {33, 34, 35}, {35, 36, 37}, {37, 38, 39}, {39, 40, 41}, {41, 42, 43},
        {45, 46, 47}, {47, 48, 49}, {50, 51, 52}, {53, 54, 55}, {55, 56, 57}, {57, 0, 2}, {2, 4, 6}, {10, 12, 14}, {14, 16, 18},
        {23, 25, 27}, {27, 29, 30}, {35, 37, 39}, {39, 41, 43}, {45, 47, 49}, {49, 50, 52}, {53, 55, 57}, {2, 6, 7}, {10, 14, 18},
        {23, 27, 30}, {33, 35, 39}, {44, 45, 49}, {53, 57, 2}, {9, 10, 18}, {22, 23, 30}, {33, 39, 43}, {44, 49, 52}, {53, 2, 7},
        {9, 18, 19}, {22, 30, 32}, {32, 33, 43}, {43, 44, 52}, {52, 53, 7}, {9, 19, 20}, {22, 32, 43}, {52, 7, 8}, {8, 9, 20},
        {22, 43, 52}, {8, 20, 21}, {21, 22, 52}, {52, 8, 21}
    };
*/
    vidx_t expected_triangles[] = {
        57, 58, 0, 0, 1, 2, 2, 3, 4, 4, 5, 6, 10, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 23, 24, 25, 25, 26, 27, 27, 28, 29, 30, 31, 32, 33, 34, 35, 35, 36, 37, 37, 38, 39, 39, 40, 41, 41, 42, 43, 45, 46, 47, 47, 48, 49, 50, 51, 52, 53, 54, 55, 55, 56, 57, 57, 0, 2, 2, 4, 6, 10, 12, 14, 14, 16, 18, 23, 25, 27, 27, 29, 30, 35, 37, 39, 39, 41, 43, 45, 47, 49, 49, 50, 52, 53, 55, 57, 2, 6, 7, 10, 14, 18, 23, 27, 30, 33, 35, 39, 44, 45, 49, 53, 57, 2, 9, 10, 18, 22, 23, 30, 33, 39, 43, 44, 49, 52, 53, 2, 7, 9, 18, 19, 22, 30, 32, 32, 33, 43, 43, 44, 52, 52, 53, 7, 9, 19, 20, 22, 32, 43, 52, 7, 8, 8, 9, 20, 22, 43, 52, 8, 20, 21, 21, 22, 52, 52, 8, 21
    };
    for (int i = 1; i <=17; ++i) {
        RUN_TESTp(rotate_test, (boxed_triangle*)expected_triangles, n, x, y, 2*M_PI/i);
    }
}

TEST funny_test(void) {
#include "funny_data.h"
    const int n = ARR_LEN(xy);
    double angle = 0;
    vertices_t vertices = dump_vertices(n, xy, angle);
    ASSERT_EQ(n, vertices_num(vertices));

    triangles_t triangles = polygon_earcut(vertices, NULL);
    ASSERT(NULL != triangles);

    polygon_t polygon = polygon_build(vertices, NULL);
    ASSERT(diff_areas(polygon, triangles) < 1e-6);

    polygon_destroy(polygon);
    triangles_free(triangles);

    PASS();
}

TEST kzer_test(void) {
#include "kzer-za.h"
    const int n = ARR_LEN(xy);
    double angle = 0;
    vertices_t vertices = dump_vertices(n, xy, angle);
    ASSERT_EQ(n, vertices_num(vertices));

    triangles_t triangles = polygon_earcut(vertices, NULL);
    ASSERT(NULL != triangles);

    polygon_t polygon = polygon_build(vertices, NULL);
    ASSERT(diff_areas(polygon, triangles) < 1e-6);

    polygon_destroy(polygon);
    triangles_free(triangles);

    PASS();
}

TEST nazca_monkey_test(void) {
    vertices_t vertices = read_vertices_from("../data/nazca_monkey.dat");

    triangles_t triangles = polygon_earcut(vertices, NULL);
    ASSERT(NULL != triangles);

    polygon_t polygon = polygon_build(vertices, NULL);
    ASSERT(diff_areas(polygon, triangles) < 1e-6);

    polygon_destroy(polygon);
    triangles_free(triangles);

    PASS();
}
TEST nazca_heron_test(void) {
    vertices_t vertices = read_vertices_from("../data/nazca_heron.dat");
    //for (int i =0; i<vertices_num(vertices); ++i) printf("%d: %g, %g\n", i, vertices_nth_getx(vertices, i), vertices_nth_gety(vertices,i));

    triangles_t triangles = polygon_earcut(vertices, NULL);
    ASSERT(NULL != triangles);

    polygon_t polygon = polygon_build(vertices, NULL);
    ASSERT(diff_areas(polygon, triangles) < 1e-5);

    polygon_destroy(polygon);
    triangles_free(triangles);

    PASS();
}

SUITE(funny_tests) {
    RUN_TEST(funny_test);
    RUN_TEST(kzer_test);
    RUN_TEST(nazca_monkey_test);
    RUN_TEST(nazca_heron_test);
    //RUN_TEST(sketchup_test);
}

TEST hole1_test(void) {
    // counter-clockwise
    const coord_t x[] = {0, 100, 100,   0, 20, 80, 80, 20};
    const coord_t y[] = {0,   0, 100, 100, 20, 20, 80, 80};

    const vidx_t holeIndices[] = {4};
    vidx_t holeNum = ARR_LEN(holeIndices);
    holes_t holes = holes_create(holeNum, holeIndices);
    DBG("holeNum: %d", holeNum);

    const vidx_t n = ARR_LEN(x);
    const vertices_t vertices = vertices_create(n, x, y);
    ASSERT_EQ_FMT(n, vertices_num(vertices), "%d");

    triangles_t triangles = polygon_earcut(vertices, holes);
    ASSERT(NULL != triangles);
int* ptr = malloc(8); (void)ptr;
    int expected_triangle_num = n + ARR_LEN(holeIndices) * 2 - 2;
    DBG("triangles.num: %d", expected_triangle_num);
    ASSERT_EQ_FMT(expected_triangle_num, triangles_num(triangles), "%d");

    const vidx_t expected_triangles[] = {
        3,0,4, 5,4,0, 3,4,7, 5,0,1, 2,3,7, 6,5,1, 2,7,6, 6,1,2
    };

    const vidx_t* tri = triangles_nth(triangles, 0);
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

SUITE(r_hole_tests) {
    coord_t x[] = {0, 100, 100,   0, 20, 80, 80, 20};
    coord_t y[] = {0,   0, 100, 100, 20, 20, 80, 80};
    const int n = ARR_LEN(x);

    vidx_t holeIndices[] = {4};

    for (int i = 1; i <=17; ++i) {
        greatest_set_test_suffix("r_hole");
        RUN_TESTp(hole_rotate_test, n, x, y, 2*M_PI/i, ARR_LEN(holeIndices), holeIndices);
    }
}

/* Add all the definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char* argv[]) {
    GREATEST_MAIN_BEGIN(); /* command-line arguments, initialization. */

//*
    RUN_SUITE(funny_tests);

    RUN_SUITE(cw_quadrangle_tests);
    RUN_SUITE(ccw_quadrangle_tests);
    RUN_SUITE(pentagon_tests);

    RUN_SUITE(hand_tests);
// */
    RUN_SUITE(i18_tests);

    RUN_SUITE(comb_tests);

    RUN_SUITE(hole_tests);
    RUN_SUITE(r_hole_tests);

    INFO("vidx_t.sz: %zu bytes, coord_t.sz: %zu bytes", sizeof(vidx_t), sizeof(coord_t));
    GREATEST_MAIN_END(); /* display results */
}

