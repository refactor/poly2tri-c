#include "mylog.h"

#include "polygon_triangulate.h"

#include "greatest.h"
#include "test_utils.h"

TEST comb_test(void) {
    #include "comb_data.h"
    const int n = ARR_LEN(x);
    ASSERT_EQ(n, ARR_LEN(y));

    vertices_t vertices = vertices_create(n, x, y);
    ASSERT_EQ(n, vertices_num(vertices));

    triangles_t triangles = polygon_triangulate(vertices);
    __auto_type m = triangles_num(triangles);
    ASSERT_EQ(n-2, m);
    
    polygon_t polygon = polygon_build(vertices, NULL);
    ASSERT( diff_areas(polygon, triangles) < 0.00001);

    const int tri_num = n - 2;
//    i4mat_transpose_print(3, tri_num, (int*)triangles, "Gotcha Comb Triangles");

    boxed_triangle* etris = (boxed_triangle*)expected_triangles;
    sort_triangles(tri_num, etris);
    __auto_type tris = triangles_nth(triangles, 0);
    boxed_triangle* btris = (boxed_triangle*)tris;
    sort_triangles(tri_num, btris);
    for (int i = 0; i < tri_num; ++i) 
        ASSERT_EQUAL_T(&etris[i], &btris[i], &boxed_triangle_type_info, NULL);

    triangles_free(triangles);
    polygon_destroy(polygon);

    PASS();
}

SUITE(the_comb_suite) {
    RUN_TEST(comb_test);
}

