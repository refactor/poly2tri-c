#include "mylog.h"

#include "polygon_triangulate.h"

#include "greatest.h"
#include "test_utils.h"

TEST comb_test(void) {
    #include "comb_data.h"
    printf("tri.len: %zu\n", ARR_LEN(expected_triangles));
    printf("tri.sz: %zu\n", sizeof(expected_triangles));
    const int n = ARR_LEN(x);
    ASSERT_EQ(n, ARR_LEN(y));

    DBG("arr.len: %u", n);
    polygon_t* polygon = allocate_polygon(n);
    for (__auto_type i=0; i<polygon->n; ++i) {
        coord_seq_setx(polygon, i, x[i]);
        coord_seq_sety(polygon, i, y[i]);
    }
    triangles_t* triangles = polygon_triangulate(polygon);
    
    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    const int tri_num = n - 2;
//    i4mat_transpose_print(3, tri_num, (int*)triangles, "Gotcha Comb Triangles");

    boxed_triangle* etris = (boxed_triangle*)expected_triangles;
    sort_triangles(tri_num, etris);
    boxed_triangle* btris = (boxed_triangle*)triangles->vidx;
    sort_triangles(tri_num, btris);
    for (int i = 0; i < tri_num; ++i) 
        ASSERT_EQUAL_T(&etris[i], &btris[i], &boxed_triangle_type_info, NULL);

    free(triangles);
    PASS();
}

SUITE(the_comb_suite) {
    RUN_TEST(comb_test);
}

