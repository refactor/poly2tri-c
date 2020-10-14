#include "polygon_triangulate.h"
#include "greatest.h"

#define ARR_LEN(arr) (sizeof(arr)/sizeof(arr[0]))

#define TRIANGLE_EQ(et, gt) ((et[0] == gt[0]) && (et[1] == gt[1]) && (et[2] == gt[2])) \
                        || ((et[0] == gt[1]) && (et[1] == gt[2]) && (et[2] == gt[0])) \
                        || ((et[0] == gt[2]) && (et[1] == gt[0]) && (et[2] == gt[1]))

typedef struct {
    int tri[3];
} boxed_triangle;

int boxed_triangle_eq(const void *exp, const void *got, void *udata) {
    const boxed_triangle *etri = (const boxed_triangle*) exp;
    const boxed_triangle *gtri = (const boxed_triangle*) got;
    (void)udata;
    return TRIANGLE_EQ(etri->tri, gtri->tri);
}
int boxed_triangle_pt(const void *got, void *udata) {
    const boxed_triangle *gtri = (const boxed_triangle*) got;
    (void)udata;
    return printf("{%d,%d,%d}", gtri->tri[0],gtri->tri[1],gtri->tri[2]);
}

greatest_type_info boxed_triangle_type_info = {
    .equal = boxed_triangle_eq,
    .print = boxed_triangle_pt
};

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

SUITE(the_suite) {
    RUN_TEST(only_one);
}

GREATEST_MAIN_DEFS();

int main(int argc, char* argv[]) {
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(the_suite);

    GREATEST_MAIN_END();
}

