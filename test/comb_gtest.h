#include "polygon_triangulate.h"

#include "greatest.h"
#include "test_utils.h"

TEST comb_test(void) {
    #include "comb_data.h"
    printf("tri.len: %zu\n", ARR_LEN(expected_triangles));
    printf("tri.sz: %zu\n", sizeof(expected_triangles));
    ASSERT_EQ(ARR_LEN(x), ARR_LEN(y));

    const int n = ARR_LEN(x);
    int* triangles = polygon_triangulate(n, x, y);
    const int tri_num = n - 2;
    i4mat_transpose_print(3, tri_num, triangles, "Gotcha Comb Triangles");

    boxed_triangle* etris = (boxed_triangle*)expected_triangles;
    sort_triangles(tri_num, etris);
    boxed_triangle* btris = (boxed_triangle*)triangles;
    sort_triangles(tri_num, btris);
    for (int i = 0; i < tri_num; ++i) 
        ASSERT_EQUAL_T(&etris[i], &btris[i], &boxed_triangle_type_info, NULL);

    free(triangles);
    PASS();
}

SUITE(the_comb_suite) {
    RUN_TEST(comb_test);
}

