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
    polygon_t* cs = allocate_coord_seq(3);
    for (__auto_type i=0; i<cs->n; ++i) {
        coord_seq_setx(cs, i, x[i]);
        coord_seq_sety(cs, i, y[i]);
    }

    DBG("cs->n: %u", cs->n);
    //reverse_polygon(cs);  // cannot do clockwise-polygon, DONOT do this
    print_polygon(cs);
    triangles_t* triangles = polygon_triangulate(cs);
    ASSERTm("MUST BE counterclockwise-polygon", triangles != NULL);

    boxed_triangle expected_triangles[] = {
        {.tri = {1,2,0}},
    };
    ASSERT_EQUAL_T(&expected_triangles, triangles->vidx, &boxed_triangle_type_info, NULL);

    free(triangles);

    PASS();
}

//*
TEST illegal_one(void) {
    //coord_t x[] = {8.0, 7.0, 7.0, 7.0};
    //coord_t y[] = {0.0, 10.0, 10.0, -10.0};
    coord_t x[] = {7.0, 7.0, 7.0, 8.0};
    coord_t y[] = {-10.0, 10.0, 10.0, 0.0};
    polygon_t* cs = allocate_coord_seq(3);
    for (__auto_type i=0; i<cs->n; ++i) {
        coord_seq_setx(cs, i, x[i]);
        coord_seq_sety(cs, i, y[i]);
    }
    triangles_t* triangles = polygon_triangulate(cs);
    ASSERT_EQ(NULL, triangles);
    
    PASS();
}
// */

TEST common_one(void) {
    //coord_t x[] = {10.0, 0.0, 60.0, 70.0};
    //coord_t y[] = {0.0, 50.0, 60.0, 10.0};
    coord_t x[] = {70.0, 60.0, 0.0, 10.0};
    coord_t y[] = {10.0, 60.0, 50.0, 0.0};
    polygon_t* cs = allocate_coord_seq(ARR_LEN(x));
    DBG("cs->n = %d", cs->n);
    for (__auto_type i=0; i<cs->n; ++i) {
        coord_seq_setx(cs, i, x[i]);
        coord_seq_sety(cs, i, y[i]);
    }
    print_polygon(cs);
    //reverse_polygon(cs);
   // print_polygon(cs);
    triangles_t* triangles = polygon_triangulate(cs);
    ASSERT(NULL != triangles);
    boxed_triangle expected_triangles[] = {
        {.tri = {1,3,0}},
        {.tri = {1,2,3}},
    };
    ASSERT_EQUAL_T(&expected_triangles, triangles->vidx, &boxed_triangle_type_info, NULL);
    (void)expected_triangles;

    free(triangles);
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
