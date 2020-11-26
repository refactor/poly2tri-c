#pragma once

#include "greatest.h"
#include "geometry_type.h"
#include "polygon_earcut.h"
#include "test_utils.h"

TEST area_eq_test(vertices_t vertices, holes_t holes) {
    const triangles_t triangles = polygon_earcut(vertices, holes);
    ASSERT(NULL != triangles);
    //vidx_t m = triangles_num(triangles);
    //ASSERT_EQ_FMT(n + 2*holes_num(holes) - 2, m, "%d");

    const polygon_t polygon = polygon_build(vertices, holes);
    ASSERT(diff_areas(polygon, triangles) < 1e-5);

    polygon_destroy(polygon);
    triangles_free(triangles);

    PASS();
}

TEST as_expected_and_area_eq_test(vertices_t vertices, holes_t holes, const int32_t tnum, const boxed_triangle expected_triangles[tnum]) {
    const triangles_t triangles = polygon_earcut(vertices, holes);
    ASSERT(NULL != triangles);
    DBG("tri -> num: %d", triangles_num(triangles));
    ASSERT_EQ_FMT(tnum, triangles_num(triangles), "%d");

    vidx_t* tri = triangles_nth(triangles, 0);
    ASSERT_EQUAL_T(expected_triangles, tri, &boxed_triangle_type_info, NULL);

    const polygon_t polygon = polygon_build(vertices, NULL);
    ASSERT(diff_areas(polygon, triangles) < EPISILON);

    polygon_destroy(polygon);
    triangles_free(triangles);

    PASS();
}
