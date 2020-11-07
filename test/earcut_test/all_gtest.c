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
    // counter-clockwise
    coord_t xs[] = {70, 60, 0, 10};
    coord_t ys[] = {10, 60, 50, 0};
    int n = ARR_LEN(xs);
    polygon_t* polygon = allocate_coord_seq(n);
    for (int i=0; i<n; ++i) {
        coord_seq_setx(polygon, i, xs[i]);
        coord_seq_sety(polygon, i, ys[i]);
    }

    triangles_t *triangles = polygon_earcut(polygon, 0, NULL);
    ASSERT(NULL != triangles);
    DBG("m=%u", triangles->m);
    ASSERT_EQ(2, triangles->m);

    boxed_triangle expected_triangles[] = {
        {.tri = {0,1,2} },
        {.tri = {2,3,0} },
    };
    ASSERT_EQUAL_T(&expected_triangles, triangles->vidx, &boxed_triangle_type_info, NULL);

    free(triangles);
    PASS();
}

TEST cw_test2(void) {
    // clockwise
    coord_t xs[] = {10,0,60,70};
    coord_t ys[] = {0,50,60,10};
    int n = ARR_LEN(xs);
    polygon_t* polygon = allocate_coord_seq(n);
    for (int i=0; i<n; ++i) {
        coord_seq_setx(polygon, i, xs[i]);
        coord_seq_sety(polygon, i, ys[i]);
    }
    triangles_t* triangles = polygon_earcut(polygon, 0, NULL);
    ASSERT(NULL != triangles);
    ASSERT_EQ(2, triangles->m);
    boxed_triangle expected_triangles[] = {
        {.tri = {3,2,1} },
        {.tri = {1,0,3} },
    };
    ASSERT_EQUAL_T(&expected_triangles, triangles->vidx, &boxed_triangle_type_info, NULL);

    free(triangles);
    PASS();
}

TEST cw_test3(void) {
    // clockwise
    coord_t xs[] = {3,5,12,9,5};
    coord_t ys[] = {4,11,8,5,6};
    int n = ARR_LEN(xs);
    polygon_t* polygon = allocate_coord_seq(n);
    for (int i=0; i<n; ++i) {
        coord_seq_setx(polygon, i, xs[i]);
        coord_seq_sety(polygon, i, ys[i]);
    }
    triangles_t* triangles = polygon_earcut(polygon, 0, NULL);
    ASSERT(NULL != triangles);
    ASSERT_EQ(3, triangles->m);
    boxed_triangle expected_triangles[] = {
        {.tri = {2,1,4} },
        {.tri = {1,0,4} },
        {.tri = {4,3,2} },
    };
    ASSERT_EQUAL_T(&expected_triangles, triangles->vidx, &boxed_triangle_type_info, NULL);

    free(triangles);
    PASS();
}

SUITE(earcut_tests) {
    RUN_TEST(ccw_test1);
    RUN_TEST(cw_test2);
    RUN_TEST(cw_test3);
}

GREATEST_MAIN_DEFS();

int main(int argc, char* argv[]) {
    INFO("vidx_t.sz: %zu bytes, coord_t.sz: %zu bytes", sizeof(vidx_t), sizeof(coord_t));

    GREATEST_MAIN_BEGIN();

    RUN_SUITE(unit_tests);
    RUN_SUITE(earcut_tests);

    GREATEST_MAIN_END();
}

