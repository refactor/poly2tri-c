#include "mylog.h"

#include "polygon_triangulate.h"

#include "greatest.h"
#include "test_utils.h"

TEST i18_test(void) {
    #include "i18_data.h"
    printf("tri.len: %zu\n", ARR_LEN(expected_triangles));
    printf("tri.sz: %zu\n", sizeof(expected_triangles));
    ASSERT_EQ(ARR_LEN(x), ARR_LEN(y));

    int n = ARR_LEN(x);
    DBG("arr.len: %u", n);
    polygon_t* cs = allocate_coord_seq(n);
    for (__auto_type i=0; i<cs->n; ++i) {
        coord_seq_setx(cs, i, x[i]);
        coord_seq_sety(cs, i, y[i]);
    }
    print_polygon(cs);
//    reverse_polygon(cs);
//    print_polygon(cs);
    triangles_t* triangles = polygon_triangulate(cs);
    ASSERT(triangles != NULL);
    const int tri_num = n - 2;
//    i4mat_transpose_print(3, tri_num, (int*)triangles, "Gotcha Triangles");

    boxed_triangle* etris = (boxed_triangle*)expected_triangles;
    sort_triangles(tri_num, etris);
    boxed_triangle* btris = (boxed_triangle*)triangles->vidx;
    sort_triangles(tri_num, btris);
    for (int i = 0; i < tri_num; ++i) 
        ASSERT_EQUAL_T(&etris[i], &btris[i], &boxed_triangle_type_info, NULL);

    free(triangles);
    PASS();
}

SUITE(the_i18_suite) {
    RUN_TEST(i18_test);
}

