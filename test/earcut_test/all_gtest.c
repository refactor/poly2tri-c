#include "mylog.h"
#include "greatest.h"
#include "test_utils.h"
#include "common_test.h"
#include "data_utils.h"
#include "polygon_earcut.h"
#include <math.h>
#include <assert.h>
#include <unistd.h>

static void copy_and_rotate(const int n, const coord_t xs[n], const coord_t ys[n], double angle, vertices_t vs) {
    double csita = cos(angle);
    double ssita = sin(angle);
    for (int i = 0; i < n; ++i) {
        coord_t tx = csita * xs[i] - ssita * ys[i];
        coord_t ty = ssita * xs[i] + csita * ys[i];
        vertices_nth_setxy(vs, i, tx, ty);
    }
}

static vertices_t rotate_vertices(const int n, const coord_t xs[n], const coord_t ys[n], double angle) {
    assert(n > 0);
    vertices_t vertices = vertices_allocate(n);
    copy_and_rotate(n, xs, ys, angle, vertices);

    return vertices;
}

TEST hole_rotate_test(const int n, const coord_t xs[n], const coord_t ys[n], double angle, const vidx_t hn, const vidx_t holeIndices[hn]) {
    const vertices_t vertices = rotate_vertices(n, xs, ys, angle);
    ASSERT_EQ(n, vertices_num(vertices));
    //for (int i=0; i<n; ++i) DBG("vertices[%2d]: %f,%f,", i, vertices_nth_getx(vertices,i),vertices_nth_gety(vertices,i));

    const holes_t holes = holes_create(hn, holeIndices);
    ASSERT_EQ(hn, holes_num(holes));
    CHECK_CALL(area_eq_test(vertices, holes));

    PASS();
}

TEST rotate_test(const int32_t tnum, const boxed_triangle expected_triangles[tnum], const int n, const coord_t xs[n], const coord_t ys[n], double angle) {
    const vertices_t vertices = rotate_vertices(n, xs, ys, angle);
    ASSERT_EQ(n, vertices_num(vertices));
/*
    const triangles_t triangles = polygon_earcut(vertices, NULL);
    ASSERT(NULL != triangles);
    ASSERT_EQ(n-2, triangles_num(triangles));

    vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(expected_triangles, tri, &boxed_triangle_type_info, NULL);
    triangles_free(triangles);

    CHECK_CALL(area_eq_test(vertices, NULL));
*/
    CHECK_CALL(as_expected_and_area_eq_test(vertices, NULL, tnum, expected_triangles));

    PASS();
}

TEST rotate_attachment_test(const int32_t tnum, const boxed_triangle expected_triangles[tnum], const int n, const coord_t xs[n], const coord_t ys[n], double angle) {
    vertices_t vertices = vertices_allocate(n);
    copy_and_rotate(n, xs, ys, angle, vertices);
/*
    const triangles_t triangles = polygon_earcut(vertices, NULL);
    ASSERT(NULL != triangles);
    ASSERT_EQ(n-2, triangles_num(triangles));

    vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(expected_triangles, tri, &boxed_triangle_type_info, NULL);

    const polygon_t polygon = polygon_build(vertices, NULL);
    ASSERT(diff_areas(polygon, triangles) < 1e-6);

    polygon_destroy(polygon);
    triangles_free(triangles);
*/
    CHECK_CALL(as_expected_and_area_eq_test(vertices, NULL, tnum, expected_triangles));
    PASS();
}
TEST attachment_test(const int32_t tnum, const boxed_triangle expected_triangles[tnum], const int n, const coord_t xs[n], const coord_t ys[n]) {
    vertices_t vertices = vertices_attach(n, xs, ys);
/*
    const triangles_t triangles = polygon_earcut(vertices, NULL);
    ASSERT(NULL != triangles);
    ASSERT_EQ(n-2, triangles_num(triangles));
    DBG("tri -> num: %d", triangles_num(triangles));

    vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(expected_triangles, tri, &boxed_triangle_type_info, NULL);

    const polygon_t polygon = polygon_build(vertices, NULL);
    ASSERT(diff_areas(polygon, triangles) < 1e-6);

    polygon_destroy(polygon);
    triangles_free(triangles);
*/
    CHECK_CALL(as_expected_and_area_eq_test(vertices, NULL, tnum, expected_triangles));
    PASS();
}

SUITE(quadr_angle) {
    // a clockwise ploygon
    const coord_t xs[] = {10,0,60,70};
    const coord_t ys[] = {0,50,60,10};
    const vidx_t n = ARR_LEN(xs);
    boxed_triangle expected_triangles[] = {
        {.tri = {1,0,3} },
        {.tri = {3,2,1} },
    };
    int32_t tnum = ARR_LEN(expected_triangles);
    RUN_TESTp(attachment_test, tnum, expected_triangles, n, xs, ys);
}
SUITE(cw_quadrangle_tests) {
    // a clockwise ploygon
    const coord_t xs[] = {10,0,60,70};
    const coord_t ys[] = {0,50,60,10};
    const vidx_t n = ARR_LEN(xs);

    boxed_triangle expected_triangles[] = {
        {.tri = {1,0,3} },
        {.tri = {3,2,1} },
    };
    int32_t tnum = ARR_LEN(expected_triangles);
    for (int i = 1; i <=11; ++i) {
        RUN_TESTp(rotate_test, tnum, expected_triangles, n, xs, ys, 2*M_PI/i);
        RUN_TESTp(rotate_attachment_test, tnum, expected_triangles, n, xs, ys, 2*M_PI/i);
    }
}

SUITE(pentagon_tests) {
    // clockwise
    const coord_t xs[] = {3,5,12,9,5};
    const coord_t ys[] = {4,11,8,5,6};
    const vidx_t n = ARR_LEN(xs);
    boxed_triangle expected_triangles[] = {
        {.tri = {1,0,4} },
        {.tri = {4,3,2} },
        {.tri = {2,1,4} },
    };
    int32_t tnum = ARR_LEN(expected_triangles);
    for (int i = 1; i <=9; ++i) {
        RUN_TESTp(rotate_test, tnum, expected_triangles, n, xs, ys, 2*M_PI/i);
        RUN_TESTp(rotate_attachment_test, tnum, expected_triangles, n, xs, ys, 2*M_PI/i);
    }
}

SUITE(ccw_quadrangle_tests) {
    //*
    // a counter-clockwise quadrangle
    const coord_t xs[] = {70, 60, 0, 10};
    const coord_t ys[] = {10, 60, 50, 0};
    const vidx_t n = ARR_LEN(xs);
    boxed_triangle expected_triangles[] = {
        {.tri = {2,3,0} },
        {.tri = {0,1,2} },
    };
    //*/
    int32_t tnum = ARR_LEN(expected_triangles);
    for (int i = 1; i <= 7; ++i) {
        greatest_set_test_suffix("ccw_quadrangle");
        RUN_TESTp(rotate_test, tnum, expected_triangles, n, xs, ys, 2*M_PI/i);
        RUN_TESTp(rotate_attachment_test, tnum, expected_triangles, n, xs, ys, 2*M_PI/i);
    }
}

SUITE(i18_tests) {
#include "i18_data.h"
    int32_t tnum = ARR_LEN(expected_triangles);
    const vidx_t n = ARR_LEN(x);
    for (int i = 1; i <=19; ++i) {
        greatest_set_test_suffix("i18");
        RUN_TESTp(rotate_test, tnum, (boxed_triangle*)expected_triangles, n, x, y, 2*M_PI/i);
        RUN_TESTp(rotate_attachment_test, tnum, (boxed_triangle*)expected_triangles, n, x, y, 2*M_PI/i);
    }
}

SUITE(comb_tests) {
#include "comb_data.h"
    int32_t tnum = ARR_LEN(expected_triangles);
    const vidx_t n = ARR_LEN(x);
    for (int i = 1; i <=19; ++i) {
        greatest_set_test_suffix("hand");
        RUN_TESTp(rotate_test, tnum, (boxed_triangle*)expected_triangles, n, x, y, 2*M_PI/i);
        RUN_TESTp(rotate_attachment_test, tnum, (boxed_triangle*)expected_triangles, n, x, y, 2*M_PI/i);
    }
}

SUITE(hand_tests) {
#include "hand_data.h"
    const int n = ARR_LEN(x);
    vidx_t expected_triangles[][3] = {
        {57, 58, 0}, {0, 1, 2}, {2, 3, 4}, {4, 5, 6}, {10, 11, 12}, {12, 13, 14}, {14, 15, 16}, {16, 17, 18}, {23, 24, 25}, 
        {25, 26, 27}, {27, 28, 29}, {30, 31, 32}, {33, 34, 35}, {35, 36, 37}, {37, 38, 39}, {39, 40, 41}, {41, 42, 43},
        {45, 46, 47}, {47, 48, 49}, {50, 51, 52}, {53, 54, 55}, {55, 56, 57}, {57, 0, 2}, {2, 4, 6}, {10, 12, 14}, {14, 16, 18},
        {23, 25, 27}, {27, 29, 30}, {35, 37, 39}, {39, 41, 43}, {45, 47, 49}, {49, 50, 52}, {53, 55, 57}, {2, 6, 7}, {10, 14, 18},
        {23, 27, 30}, {33, 35, 39}, {44, 45, 49}, {53, 57, 2}, {9, 10, 18}, {22, 23, 30}, {33, 39, 43}, {44, 49, 52}, {53, 2, 7},
        {9, 18, 19}, {22, 30, 32}, {32, 33, 43}, {43, 44, 52}, {52, 53, 7}, {9, 19, 20}, {22, 32, 43}, {52, 7, 8}, {8, 9, 20},
        {22, 43, 52}, {8, 20, 21}, {21, 22, 52}, {52, 8, 21}
    };
    /*
    vidx_t expected_triangles[] = {
        57, 58, 0, 0, 1, 2, 2, 3, 4, 4, 5, 6, 10, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 23, 24, 25, 25, 26, 27, 27, 28, 29, 30, 31, 32, 33, 34, 35, 35, 36, 37, 37, 38, 39, 39, 40, 41, 41, 42, 43, 45, 46, 47, 47, 48, 49, 50, 51, 52, 53, 54, 55, 55, 56, 57, 57, 0, 2, 2, 4, 6, 10, 12, 14, 14, 16, 18, 23, 25, 27, 27, 29, 30, 35, 37, 39, 39, 41, 43, 45, 47, 49, 49, 50, 52, 53, 55, 57, 2, 6, 7, 10, 14, 18, 23, 27, 30, 33, 35, 39, 44, 45, 49, 53, 57, 2, 9, 10, 18, 22, 23, 30, 33, 39, 43, 44, 49, 52, 53, 2, 7, 9, 18, 19, 22, 30, 32, 32, 33, 43, 43, 44, 52, 52, 53, 7, 9, 19, 20, 22, 32, 43, 52, 7, 8, 8, 9, 20, 22, 43, 52, 8, 20, 21, 21, 22, 52, 52, 8, 21
    };
*/
    int32_t tnum = ARR_LEN(expected_triangles);
    for (int i = 1; i <=17; ++i) {
        RUN_TESTp(rotate_test, tnum, (boxed_triangle*)expected_triangles, n, x, y, 2*M_PI/i);
        RUN_TESTp(rotate_attachment_test, tnum, (boxed_triangle*)expected_triangles, n, x, y, 2*M_PI/i);
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

TEST test_1hole(void) {
    // counter-clockwise
    const coord_t x[] = {0, 100, 100,   0, 20, 80, 80, 20};
    const coord_t y[] = {0,   0, 100, 100, 20, 20, 80, 80};

    const vidx_t holeIndices[] = {4};
    vidx_t holeNum = ARR_LEN(holeIndices);
    holes_t holes = holes_create(holeNum, holeIndices);

    const vidx_t n = ARR_LEN(x);
    const vertices_t vertices = vertices_create(n, x, y);
    ASSERT_EQ_FMT(n, vertices_num(vertices), "%d");

    triangles_t triangles = polygon_earcut(vertices, holes);
    ASSERT(NULL != triangles);

    int expected_triangle_num = n + ARR_LEN(holeIndices) * 2 - 2;
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

TEST test_2holesa(void) {
    const coord_t x[] = {0, 0,25,65,100,90,80,50,45, 10,40,42,30,10, 72,60,45};
    const coord_t y[] = {0,40,75,90, 80,10, 0,25, 0, 10,10,50,60,30, 65,85,70};

    const vidx_t holeIndices[] = {9,14};
    vidx_t holeNum = ARR_LEN(holeIndices);
    holes_t holes = holes_create(holeNum, holeIndices);

    const vidx_t n = ARR_LEN(x);
    const vertices_t vertices = vertices_create(n, x, y);
    ASSERT_EQ_FMT(n, vertices_num(vertices), "%d");
    ASSERT_EQ(17, n);

    triangles_t triangles = polygon_earcut(vertices, holes);
    ASSERT(NULL != triangles);
//DBG("last: (%d,%d,%d)", triangles_nth(triangles, 21)[0], triangles_nth(triangles,21)[1], triangles_nth(triangles,21)[2]);

    int expected_triangle_num = n + ARR_LEN(holeIndices) * 2 - 2;
    ASSERT_EQ_FMT(expected_triangle_num, triangles_num(triangles), "%d");

    const vidx_t expected_triangles[] = {
        12, 16, 15, 10, 9, 0, 7, 6, 5, 5, 4, 3, 3, 2, 1, 1, 0, 9, 10, 0, 8, 1, 9, 13, 11, 10, 8, 1, 13, 12, 14, 16, 12, 11, 8, 7, 3, 1, 12, 14, 12, 11, 11, 7, 5, 3, 12, 15, 14, 11, 5, 3, 15, 14, 14, 5, 3
    };

    const vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(expected_triangles, tri, &boxed_triangle_type_info, NULL);

    polygon_t polygon = polygon_build(vertices, holes);
    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    triangles_free(triangles);
    polygon_destroy(polygon);

    PASS();
}
TEST test_1holeb(void) {
    const coord_t x[] = {0, 0,25,65,100,90,80,50,45, 75,70,75,80};
    const coord_t y[] = {0,40,75,90, 80,10, 0,25, 0, 55,45,20,50};

    const vidx_t holeIndices[] = {9};
    vidx_t holeNum = ARR_LEN(holeIndices);
    holes_t holes = holes_create(holeNum, holeIndices);

    const vidx_t n = ARR_LEN(x);
    const vertices_t vertices = vertices_create(n, x, y);
    ASSERT_EQ_FMT(n, vertices_num(vertices), "%d");
    ASSERT_EQ_FMT(13, n, "%d");

    triangles_t triangles = polygon_earcut(vertices, holes);
    ASSERT(NULL != triangles);
//DBG("last: (%d,%d,%d)", triangles_nth(triangles, 21)[0], triangles_nth(triangles,21)[1], triangles_nth(triangles,21)[2]);

    int expected_triangle_num = n + ARR_LEN(holeIndices) * 2 - 2;
    ASSERT_EQ_FMT(expected_triangle_num, triangles_num(triangles), "%d");

    const vidx_t expected_triangles[] = {
        0,8,7, 7,6,5, 4,3,2, 2,1,10, 11,10,1, 1,0,7, 2,10,9, 11,1,7, 4,2,9, 11,7,5, 4,9,12, 12,11,5, 5,4,12
    };

    const vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(expected_triangles, tri, &boxed_triangle_type_info, NULL);

    polygon_t polygon = polygon_build(vertices, holes);
    ASSERT( diff_areas(polygon, triangles) < EPISILON);

    triangles_free(triangles);
    polygon_destroy(polygon);

    PASS();
}
TEST test_2holesb(void) {
    const coord_t x[] = {0, 0,25,65,100,90,80,50,45, 10,40,42,30,10, 75,70,75,80};
    const coord_t y[] = {0,40,75,90, 80,10, 0,25, 0, 10,10,50,60,30, 55,45,20,50};

    const vidx_t holeIndices[] = {9,14};
    vidx_t holeNum = ARR_LEN(holeIndices);
    holes_t holes = holes_create(holeNum, holeIndices);

    const vidx_t n = ARR_LEN(x);
    const vertices_t vertices = vertices_create(n, x, y);
    ASSERT_EQ_FMT(n, vertices_num(vertices), "%d");
    ASSERT_EQ(18, n);

    triangles_t triangles = polygon_earcut(vertices, holes);
    ASSERT(NULL != triangles);

    int expected_triangle_num = n + ARR_LEN(holeIndices) * 2 - 2;
    ASSERT_EQ_FMT(expected_triangle_num, triangles_num(triangles), "%d");

    const vidx_t expected_triangles[] = {
        10,9,0, 7,15,14, 6,5,4, 4,3,2, 1,0,9, 10,0,8, 1,9,13, 11,10,8, 2,1,13, 11,8,7, 16,15,7, 2,13,12, 11,7,14, 16,7,6, 4,2,12,
        12,11,14, 17,16,6, 4,12,14, 17,6,4, 4,14,17
    };

    const vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(expected_triangles, tri, &boxed_triangle_type_info, NULL);

    polygon_t polygon = polygon_build(vertices, holes);
    ASSERT( diff_areas(polygon, triangles) < EPISILON);

    triangles_free(triangles);
    polygon_destroy(polygon);

    PASS();
}
TEST test_ill3holes(void) {
    const coord_t x[] = {
          0.000000, -20.000000, -15.849365, 11.291651, 46.602539, 72.942284, 69.282036, 30.801270, 38.971142, 3.660254, 29.641016, 11.373067, -4.019238, -6.339746, 29.853828, 9.461524, 3.971143, 37.451904, 38.121777, 54.951904, 44.282032,
    };
    const coord_t y[] = {
      0.000000, 34.641018, 77.451904, 110.442284, 119.282036, 53.660255, 40.000000, 46.650635, 22.500000, 13.660254, 28.660254, 64.301270, 66.961525, 30.980762, 92.291649, 103.61216, 83.121780, 85.131393, 73.971146, 54.820507, 83.301270,
    };
    const vidx_t n = ARR_LEN(x);
    const vertices_t vertices = vertices_create(n, x, y);

    const vidx_t holeIndices[] = {9,14,17};
    vidx_t holeNum = ARR_LEN(holeIndices);
    holes_t holes = holes_create(holeNum, holeIndices);
    
    const vidx_t expected_triangles[] = {
        7, 6, 5, 2, 16, 15, 14, 16, 17, 18, 17, 16, 16, 2, 1, 1, 13, 12, 3, 2, 15, 14, 17, 20, 19, 18, 16, 4, 3, 15, 4, 15, 14, 4, 14, 20, 5, 4, 20, 5, 20, 19, 7, 5, 19, 7, 19, 16, 8, 7, 16, 16, 12, 11, 9, 13, 1, 8, 16, 11, 9, 1, 0, 8, 11, 10, 10, 9, 0, 0, 8, 10
    };

    triangles_t triangles = polygon_earcut(vertices, holes);
    ASSERT(NULL != triangles);

    const vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(expected_triangles, tri, &boxed_triangle_type_info, NULL);

    polygon_t polygon = polygon_build(vertices, holes);
    ASSERT( diff_areas(polygon, triangles) < EPISILON);

    triangles_free(triangles);
    polygon_destroy(polygon);

    PASS();
}
//*
TEST test_3holes(const boxed_triangle* expected_triangles, const vertices_t vertices, const holes_t holes) {
    const int n = vertices_num(vertices);
    ASSERT_EQ_FMT(n, vertices_num(vertices), "%d");

    triangles_t triangles = polygon_earcut(vertices, holes);
    ASSERT(NULL != triangles);

    int expected_triangle_num = n + holes_num(holes) * 2 - 2;
    WARN("expected triangles SHOULD BE: %d, NOT: %d", expected_triangle_num-1, expected_triangle_num);
//    ASSERT_EQ_FMT(expected_triangle_num, triangles_num(triangles), "%d");

    const vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(expected_triangles, tri, &boxed_triangle_type_info, NULL);

    polygon_t polygon = polygon_build(vertices, holes);
    ASSERT( diff_areas(polygon, triangles) < EPISILON);

    triangles_free(triangles);
    polygon_destroy(polygon);

    PASS();
}
// */
SUITE(suite_3holesa) {
    const coord_t x[] = {0, 0,25,65,100,90,80,50,45, 10,40,42,30,10, 72,60,45, 75,70,75,80};
    const coord_t y[] = {0,40,75,90, 80,10, 0,25, 0, 10,10,50,60,30, 65,85,70, 55,45,20,50};
    const vidx_t n = ARR_LEN(x);

    const vidx_t holeIndices[] = {9,14,17};

    //*
    vidx_t holeNum = ARR_LEN(holeIndices);
    holes_t holes = holes_create(holeNum, holeIndices);

    const vertices_t vertices = vertices_attach(n, x, y);

    const vidx_t expected_triangles[] = {
        12,16,15, 10,9,0, 7,18,17, 6,5,4, 3,2,1, 1,0,9, 10,0,8, 1,9,13, 11,10,8, 1,13,12, 14,16,12, 11,8,7, 19,18,7, 3,1,12, 14,12,11,
        11,7,17, 19,7,6, 3,12,15, 14,11,17, 20,19,6, 4,3,15, 14,17,20, 20,6,4, 4,15,14, 14,20,4
    };

    RUN_TESTp(test_3holes, (const boxed_triangle*)expected_triangles, vertices, holes);
    for (int i = 1; i < 29; ++i) {
        RUN_TESTp(hole_rotate_test, n, x, y, 2*M_PI/i, ARR_LEN(holeIndices), holeIndices);
        DBG("pass %d", i);
    }
// */
}
SUITE(suite_3holesb) {
    /*
    const coord_t x[] = {
          0.000000, -20.000000, -15.849365, 11.291651, 46.602539, 72.942284, 69.282036, 30.801270, 38.971142, 3.660254, 29.641016, 11.373067, -4.019238, -6.339746, 29.853828, 9.461524, 3.971143, 37.451904, 38.121777, 54.951904, 44.282032,
    };
    const coord_t y[] = {
      0.000000, 34.641018, 77.451904, 110.442284, 119.282036, 53.660255, 40.000000, 46.650635, 22.500000, 13.660254, 28.660254, 64.301270, 66.961525, 30.980762, 92.291649, 103.61216, 83.121780, 85.131393, 73.971146, 54.820507, 83.301270,
    };

    const vidx_t holeIndices[] = {9,14,17};
    const vidx_t n = ARR_LEN(x);

    RUN_TESTp(hole_rotate_test, n, x, y, 0, ARR_LEN(holeIndices), holeIndices);
    */
    RUN_TEST(test_ill3holes);
}
// */
SUITE(hole_tests) {
    RUN_TEST(test_1hole);
    RUN_TEST(test_1holeb);
    RUN_TEST(test_2holesa);
    RUN_TEST(test_2holesb);
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
//*
#include "polygon_generator.h"

TEST random_polygon_test(const vidx_t num) {
    //const vidx_t num = 30000;
    vertices_t res = polygon_generate(num);
    __auto_type sa = signed_area(res, 0, num);
    DBG("area: %f", sa);
    ASSERT( sa > 0.0);
    CHECK_CALL(area_eq_test(res, NULL));

//    vertices_destroy(res);
    PASS();
}

SUITE(random_polygons) {
    for (int i = 10000; i <= 30000; i += 10000) {
        RUN_TEST1(random_polygon_test, i);
    }
}
// */

/* Add all the definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char* argv[]) {
    GREATEST_MAIN_BEGIN(); /* command-line arguments, initialization. */

//*
    RUN_SUITE(funny_tests);

    RUN_SUITE(quadr_angle);
    RUN_SUITE(cw_quadrangle_tests);
    RUN_SUITE(ccw_quadrangle_tests);
    RUN_SUITE(pentagon_tests);

    RUN_SUITE(hand_tests);
// */
    RUN_SUITE(i18_tests);

    RUN_SUITE(comb_tests);

    RUN_SUITE(hole_tests);
    RUN_SUITE(suite_3holesa);
    DBG("Pass suite_3holesa");
    RUN_SUITE(suite_3holesb);
    RUN_SUITE(r_hole_tests);

    RUN_SUITE(random_polygons);
    INFO("vidx_t.sz: %zu bytes, coord_t.sz: %zu bytes", sizeof(vidx_t), sizeof(coord_t));
    GREATEST_MAIN_END(); /* display results */
}

