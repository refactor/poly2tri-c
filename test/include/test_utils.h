#pragma once

#include <stdlib.h>
#include <stdio.h>

#include "geometry_type.h"
#include "fmt_polygon_triangulate.h"

#include "greatest.h"

#define IDX_MIN(a, b) ((a) < (b) ? (a) : (b))

#define ARR_LEN(arr) (sizeof(arr)/sizeof(arr[0]))

#define TRIANGLE_EQ(et, gt) ((et[0] == gt[0]) && (et[1] == gt[1]) && (et[2] == gt[2])) \
                        || ((et[0] == gt[1]) && (et[1] == gt[2]) && (et[2] == gt[0])) \
                        || ((et[0] == gt[2]) && (et[1] == gt[0]) && (et[2] == gt[1]))

typedef struct {
    vidx_t tri[3];
} boxed_triangle;


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

void reverse_polygon(polygon_t poly) {
    vidx_t vnum = vertices_num(poly);
    for (int i = 0; i < vnum / 2; ++i) {
        coord_t tx = coord_seq_getx(poly, i);
        coord_t ty = coord_seq_gety(poly, i);

        coord_seq_setx(poly, i, coord_seq_getx(poly, vnum - 1 - i));
        coord_seq_sety(poly, i, coord_seq_gety(poly, vnum - 1 - i));

        coord_seq_setx(poly, vnum - 1 - i, tx);
        coord_seq_sety(poly, vnum - 1 - i, ty);
    }
}

coord_t diff_areas(polygon_t polygon, triangles_t triangles) {
    __auto_type darea = signed_area(polygon);
    __typeof__(darea) areas = 0;
    __auto_type m = triangles_num(triangles);
    for (__auto_type i = 0; i < m; ++i) {
        vidx_t* tri = triangles_nth(triangles, i);
        coord_t ax = coord_seq_getx(polygon, tri[0]);
        coord_t ay = coord_seq_gety(polygon, tri[0]);
        coord_t bx = coord_seq_getx(polygon, tri[1]);
        coord_t by = coord_seq_gety(polygon, tri[1]);
        coord_t cx = coord_seq_getx(polygon, tri[2]);
        coord_t cy = coord_seq_gety(polygon, tri[2]);
        areas += triangle_area(ax,ay, bx,by, cx,cy);
    }
    DBG("signed_area: %g, triangles-areas: %g", darea, areas);
    __auto_type diff = darea - areas;
    return THE_ABS(diff);
}

void print_polygon(polygon_t poly) {
    vidx_t vnum = vertices_num(poly);
    printf("polygon n=%d\n\t", vnum);
    for (int i=0; i<vnum; ++i) {
        printf("%d:[%g,%g], ", i, coord_seq_getx(poly, i), coord_seq_gety(poly, i));
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
