#pragma once

#include <stdlib.h>
#include <stdio.h>

#include "fmt_polygon_triangulate.h"

#include "greatest.h"

#define IDX_MIN(a, b) ((a) < (b) ? (a) : (b))

#define ARR_LEN(arr) (sizeof(arr)/sizeof(arr[0]))

#define TRIANGLE_EQ(et, gt) ((et[0] == gt[0]) && (et[1] == gt[1]) && (et[2] == gt[2])) \
                        || ((et[0] == gt[1]) && (et[1] == gt[2]) && (et[2] == gt[0])) \
                        || ((et[0] == gt[2]) && (et[1] == gt[0]) && (et[2] == gt[1]))

typedef struct {
    int tri[3];
} boxed_triangle;


int boxed_triangle_min(const boxed_triangle* btri) {
    return IDX_MIN(IDX_MIN(btri->tri[0], btri->tri[1]), btri->tri[2]);
}
int boxed_triangle_baseidx(const boxed_triangle* btri) {
    int minidx = boxed_triangle_min(btri);
    int i = 0;
    for (; i < 3; ++i)
        if (minidx == btri->tri[i]) break;
    return i;
}

int boxed_triangle_eq(const void *exp, const void *got, void *udata) {
    const boxed_triangle *etri = (const boxed_triangle*) exp;
    int ebi = boxed_triangle_baseidx(etri);
    const boxed_triangle *gtri = (const boxed_triangle*) got;
    int gbi = boxed_triangle_baseidx(gtri);
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
    int bi1 = boxed_triangle_baseidx(tri1);
    const boxed_triangle *tri2 = (const boxed_triangle*) d2;
    int bi2 = boxed_triangle_baseidx(tri2);
    int res = tri1->tri[bi1+0] - tri2->tri[bi2+0];
    if (res == 0) {
        res = tri1->tri[(bi1+1)%3] - tri2->tri[(bi2+1)%3];
        if (res == 0) {
            res = tri1->tri[(bi1+2)%3] - tri2->tri[(bi2+2)%3];
        }
    }
    return res;
}

void sort_triangles(int num, boxed_triangle triangles[num]) {
    qsort(triangles, num, sizeof(boxed_triangle), triangle_cmp);
}
