#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdlib.h>
#include <stdio.h>

#include "mylog.h"
#include "geometry_type.h"

#include "greatest.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288)
#endif

#define EPISILON 0.000001

#define IDX_MIN(a, b) ((a) < (b) ? (a) : (b))

#define ARR_LEN(arr) ((vidx_t)(sizeof(arr)/sizeof(arr[0])))

#define TRIANGLE_EQ(et, gt) ((et[0] == gt[0]) && (et[1] == gt[1]) && (et[2] == gt[2])) \
                        || ((et[0] == gt[1]) && (et[1] == gt[2]) && (et[2] == gt[0])) \
                        || ((et[0] == gt[2]) && (et[1] == gt[0]) && (et[2] == gt[1]))

typedef struct {
    vidx_t tri[3];
} boxed_triangle;

vidx_t boxed_triangle_min(const boxed_triangle* btri);
vidx_t boxed_triangle_baseidx(const boxed_triangle* btri);
int boxed_triangle_eq(const void *exp, const void *got, void *udata);
int boxed_triangle_pt(const void *got, void *udata);
int triangle_cmp(const void *d1, const void *d2);
void print_triangles(triangles_t triangles);
void print_polygon(vertices_t poly);
coord_t diff_areas(polygon_t polygon, triangles_t triangles);
void reverse_polygon(vertices_t poly);
void sort_triangles(int num, boxed_triangle triangles[num]);

extern greatest_type_info boxed_triangle_type_info;
#endif // TEST_UTILS_H

#ifdef TEST_UTILS_IMPLEMENTATION

vidx_t boxed_triangle_min(const boxed_triangle* btri) {
    return IDX_MIN(IDX_MIN(btri->tri[0], btri->tri[1]), btri->tri[2]);
}
vidx_t boxed_triangle_baseidx(const boxed_triangle* btri) {
    vidx_t minidx = boxed_triangle_min(btri);
    vidx_t i = 0;
    for (; i < 3; ++i)
        if (minidx == btri->tri[i]) break;
    return i;
}

int boxed_triangle_eq(const void *exp, const void *got, void *udata) {
    const boxed_triangle *etri = (const boxed_triangle*) exp;
    vidx_t ebi = boxed_triangle_baseidx(etri);
    const boxed_triangle *gtri = (const boxed_triangle*) got;
    vidx_t gbi = boxed_triangle_baseidx(gtri);
    (void)udata;
    return etri->tri[ebi] == gtri->tri[gbi]
        && etri->tri[(ebi+1)%3] == gtri->tri[(gbi+1)%3]
        && etri->tri[(ebi+2)%3] == gtri->tri[(gbi+2)%3];
}
int boxed_triangle_pt(const void *got, void *udata) {
    const boxed_triangle *gtri = (const boxed_triangle*) got;
    (void)udata;
    return printf("triagle{%d,%d,%d}", gtri->tri[0],gtri->tri[1],gtri->tri[2]);
}

greatest_type_info boxed_triangle_type_info = {
    .equal = boxed_triangle_eq,
    .print = boxed_triangle_pt
};

int triangle_cmp(const void *d1, const void *d2) {
    const boxed_triangle *tri1 = (const boxed_triangle*) d1;
    vidx_t bi1 = boxed_triangle_baseidx(tri1);
    const boxed_triangle *tri2 = (const boxed_triangle*) d2;
    vidx_t bi2 = boxed_triangle_baseidx(tri2);
    int res = (int)tri1->tri[bi1+0] - (int)tri2->tri[bi2+0];
    if (res == 0) {
        res = (int)tri1->tri[(bi1+1)%3] - (int)tri2->tri[(bi2+1)%3];
        if (res == 0) {
            res = (int)tri1->tri[(bi1+2)%3] - (int)tri2->tri[(bi2+2)%3];
        }
    }
    return res;
}

void sort_triangles(int num, boxed_triangle triangles[num]) {
    qsort(triangles, num, sizeof(boxed_triangle), triangle_cmp);
}

void reverse_polygon(vertices_t poly) {
    vidx_t vnum = vertices_num(poly);
    for (int i = 0; i < vnum / 2; ++i) {
        coord_t tx = vertices_nth_getx(poly, i);
        coord_t ty = vertices_nth_gety(poly, i);

        vertices_nth_setxy(poly, i, vertices_nth_getx(poly, vnum - 1 - i), vertices_nth_gety(poly, vnum - 1 - i));

        vertices_nth_setxy(poly, vnum - 1 - i, tx, ty);
    }
}

coord_t diff_areas(polygon_t polygon, triangles_t triangles) {
    __auto_type darea = polygon_area(polygon);
    double areas = 0;
    __auto_type m = triangles_num(triangles);
    vertices_t vertices = polygon_getvertices(polygon);
    for (__auto_type i = 0; i < m; ++i) {
        vidx_t* tri = triangles_nth(triangles, i);
        coord_t ax = vertices_nth_getx(vertices, tri[0]);
        coord_t ay = vertices_nth_gety(vertices, tri[0]);
        coord_t bx = vertices_nth_getx(vertices, tri[1]);
        coord_t by = vertices_nth_gety(vertices, tri[1]);
        coord_t cx = vertices_nth_getx(vertices, tri[2]);
        coord_t cy = vertices_nth_gety(vertices, tri[2]);
        areas += THE_ABS(triangle_area(ax,ay, bx,by, cx,cy));
    }
    __auto_type diff = 1 - areas/darea;
    DBG("polygon[#%d].area: %.4f - triangles[#%d].area: %.4f = %g", vertices_num(vertices), darea, m, areas, diff);
    return THE_ABS(diff);
}

void print_polygon(vertices_t poly) {
    vidx_t vnum = vertices_num(poly);
    printf("polygon n=%d\n\t", vnum);
    for (int i=0; i<vnum; ++i) {
        printf("%d:[%g,%g], ", i, vertices_nth_getx(poly, i), vertices_nth_gety(poly, i));
    }
    printf("\n");
}

void print_triangles(triangles_t triangles) {
    __auto_type m = triangles_num(triangles);
    printf("triangles #%d\n", m);
    for (int i=0; i<m; ++i) {
        vidx_t* tri = triangles_nth(triangles, i);
        printf("\t%d:{%d, %d, %d}\n", i, tri[0], tri[1], tri[2]);
    }
    printf("\n");
}

#endif // TEST_UTILS_IMPLEMENTATION
