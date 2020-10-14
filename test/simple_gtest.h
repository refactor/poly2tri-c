#include "polygon_triangulate.h"

#include "greatest.h"
#include "test_utils.h"


TEST only_one(void) {
    double x[] = {8.0, 7.0, 6.0, 7.0};
    double y[] = {0.0, 10.0, 0.0, -10.0};

    int n = 3;//ARR_LEN(x);
    int* triangles = polygon_triangulate(n, x, y);
    i4mat_transpose_print(3, n-2, triangles, "Only One Triangle");

    boxed_triangle expected_triangles[] = {
        {1,2,0},
    };
    ASSERT_EQUAL_T(&expected_triangles, triangles, &boxed_triangle_type_info, NULL);

    free(triangles);

    PASS();
}

TEST illegal_one(void) {
    double x[] = {8.0, 7.0, 7.0, 7.0};
    double y[] = {0.0, 10.0, 10.0, -10.0};
    int n = ARR_LEN(x);
    int* triangles = polygon_triangulate(n, x, y);
    ASSERT_EQ(NULL, triangles);
    
    PASS();
}

SUITE(simple_suite) {
    RUN_TEST(only_one);
    RUN_TEST(illegal_one);
}

