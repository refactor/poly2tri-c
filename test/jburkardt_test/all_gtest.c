#ifndef __APPLE__
#define _GNU_SOURCE
#endif
#include "mylog.h"
#include "greatest.h"

#include "i18_gtest.h"
#include "comb_gtest.h"
#include "hand_gtest.h"

#include "polygon_triangulate.h"

#include "test_utils.h"
TEST only_one(void) {
    coord_t x[] = {8.0, 7.0, 6.0, 7.0};
    coord_t y[] = {1.0, 10.0, 0.0, -10.0};
    DBG("coord_t.sz: %zu", sizeof(coord_t));
    DBG("x.sz: %zu, y.sz: %zu", sizeof(x), sizeof(y));
    const int n = 3;
    polygon_t polygon = allocate_polygon(n);
    for (__auto_type i=0; i<3; ++i) {
        coord_seq_setx(polygon, i, x[i]);
        coord_seq_sety(polygon, i, y[i]);
    }
    ASSERT_EQ(n, vertices_num(polygon));

    //reverse_polygon(polygon);  // cannot do clockwise-polygon, DONOT do this
    print_polygon(polygon);
    triangles_t triangles = polygon_triangulate(polygon);
    ASSERTm("MUST BE counterclockwise-polygon", triangles != NULL);
    __auto_type m = triangles_num(triangles);
    ASSERT_EQ(n - 2, m);

    boxed_triangle expected_triangles[] = {
        {.tri = {1,2,0}},
    };
    __auto_type tris = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(&expected_triangles, tris, &boxed_triangle_type_info, NULL);

    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    free_triangles(triangles);
    free_polygon(polygon);

    PASS();
}

//*
TEST illegal_one(void) {
    //coord_t x[] = {8.0, 7.0, 7.0, 7.0};
    //coord_t y[] = {0.0, 10.0, 10.0, -10.0};
    coord_t x[] = {7.0, 7.0, 7.0, 8.0};
    coord_t y[] = {-10.0, 10.0, 10.0, 0.0};
    const int n = 3;
    polygon_t polygon = allocate_polygon(n);
    for (__auto_type i=0; i<n; ++i) {
        coord_seq_setx(polygon, i, x[i]);
        coord_seq_sety(polygon, i, y[i]);
    }
    ASSERT_EQ(n, vertices_num(polygon));

    triangles_t triangles = polygon_triangulate(polygon);
    ASSERT_EQ(NULL, triangles);
    
    PASS();
}
// */

TEST common_one(void) {
    //coord_t x[] = {10.0, 0.0, 60.0, 70.0};
    //coord_t y[] = {0.0, 50.0, 60.0, 10.0};
    coord_t x[] = {70.0, 60.0, 0.0, 10.0};
    coord_t y[] = {10.0, 60.0, 50.0, 0.0};
    const int n = ARR_LEN(x);
    polygon_t polygon = allocate_polygon(n);
    for (__auto_type i=0; i<n; ++i) {
        coord_seq_setx(polygon, i, x[i]);
        coord_seq_sety(polygon, i, y[i]);
    }
    ASSERT_EQ(n, vertices_num(polygon));

    //print_polygon(polygon);
    //reverse_polygon(polygon);
   // print_polygon(polygon);
    triangles_t triangles = polygon_triangulate(polygon);
    ASSERT(NULL != triangles);
    __auto_type m = triangles_num(triangles);
    ASSERT_EQ(n - 2, m);

    boxed_triangle expected_triangles[] = {
        {.tri = {1,3,0}},
        {.tri = {1,2,3}},
    };
    __auto_type tris = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(&expected_triangles, tris, &boxed_triangle_type_info, NULL);
    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    free_triangles(triangles);
    free_polygon(polygon);
    PASS();
}
SUITE(simple_suite) {
    RUN_TEST(only_one);
    RUN_TEST(illegal_one);

    RUN_TEST(common_one);
}

GREATEST_MAIN_DEFS();

int main(int argc, char* argv[]) {
    WARN("vidx_t.sz: %zu bytes, coord_t.sz: %zu bytes", sizeof(vidx_t), sizeof(coord_t));

    GREATEST_MAIN_BEGIN();

    RUN_SUITE(simple_suite);

    RUN_SUITE(the_i18_suite);
    RUN_SUITE(the_comb_suite);
    RUN_SUITE(the_hand_suite);

    GREATEST_MAIN_END();
}
