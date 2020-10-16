#include "../src/mylog.h"
#include "polygon_triangulate.h"

#include "greatest.h"
#include "test_utils.h"


TEST only_one(void) {
    coord_t x[] = {8.0, 7.0, 6.0, 7.0};
    coord_t y[] = {1.0, 10.0, 0.0, -10.0};
    DBG("coord_t.sz: %zu", sizeof(coord_t));
    DBG("x.sz: %zu, y.sz: %zu", sizeof(x), sizeof(y));
    coord_seq_t* cs = allocate_coord_seq(3);
    //memcpy(cs->coord_seq,       x, cs->n * sizeof(coord_t));
    //memcpy(cs->coord_seq+cs->n, y, cs->n * sizeof(coord_t));
    for (uint32_t i=0; i<cs->n; ++i) {
        coord_seq_setx(cs, i, x[i]);
        coord_seq_sety(cs, i, y[i]);
    }

    DBG("cs->n: %u", cs->n);
    for (uint32_t i=0; i<cs->n; ++i) DBG("x[%u] = %f, y[%u] = %f", i, coord_seq_getx(cs, i),i,coord_seq_gety(cs,i));
    vidx_t* triangles = polygon_triangulate(cs);
//    i4mat_transpose_print(3, n-2, (int*)triangles, "Only One Triangle");

    boxed_triangle expected_triangles[] = {
        {.tri = {1,2,0}},
    };
    ASSERT_EQUAL_T(&expected_triangles, triangles, &boxed_triangle_type_info, NULL);

    free(triangles);

    PASS();
}

//*
TEST illegal_one(void) {
    coord_t x[] = {8.0, 7.0, 7.0, 7.0};
    coord_t y[] = {0.0, 10.0, 10.0, -10.0};
    coord_seq_t* cs = allocate_coord_seq(3);
    for (uint32_t i=0; i<cs->n; ++i) {
        coord_seq_setx(cs, i, x[i]);
        coord_seq_sety(cs, i, y[i]);
    }
    vidx_t* triangles = polygon_triangulate(cs);
    ASSERT_EQ(NULL, triangles);
    
    PASS();
}
// */
SUITE(simple_suite) {
    RUN_TEST(only_one);
    RUN_TEST(illegal_one);
}

