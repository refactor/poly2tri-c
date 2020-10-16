#include "../src/mylog.h"
#include "polygon_triangulate.h"

#include "greatest.h"
#include "test_utils.h"

TEST hand_test(void) {
    #include "hand_data.h"
    printf("tri.len: %zu\n", ARR_LEN(expected_triangles));
    printf("tri.sz: %zu\n", sizeof(expected_triangles));
    const int n = ARR_LEN(x);
    ASSERT_EQ(n, ARR_LEN(y));
    DBG("arr.len: %u", n);
    coord_seq_t* cs = allocate_coord_seq(n);
    for (uint32_t i=0; i<cs->n; ++i) {
        coord_seq_setx(cs, i, x[i]);
        coord_seq_sety(cs, i, y[i]);
    }

    vidx_t* triangles = polygon_triangulate(cs);
    const int tri_num = n - 2;
//    i4mat_transpose_print(3, tri_num, (int*)triangles, "Gotcha hand Triangles");

    boxed_triangle* etris = (boxed_triangle*)expected_triangles;
    sort_triangles(tri_num, etris);
    boxed_triangle* btris = (boxed_triangle*)triangles;
    sort_triangles(tri_num, btris);
    for (int i = 0; i < tri_num; ++i) 
        ASSERT_EQUAL_T(&etris[i], &btris[i], &boxed_triangle_type_info, NULL);

    free(triangles);
    PASS();
}

SUITE(the_hand_suite) {
    RUN_TEST(hand_test);
}

