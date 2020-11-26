#ifndef POLYGON_GENERATOR_H
#define POLYGON_GENERATOR_H

#include "geometry_type.h" 

vertices_t polygon_generate(vidx_t num);

#endif  // POLYGON_GENERATOR_H


#ifdef POLYGON_GENERATOR_IMPLEMENTAION
#include <stdlib.h>

struct angle_ix {
    vidx_t i;
    coord_t angle;
};

int angle_comp(const void *a1, const void *a2) {
    struct angle_ix * ai1 = (struct angle_ix*)a1;
    struct angle_ix * ai2 = (struct angle_ix*)a2;
    if (ai1->angle < ai2->angle) return -1;
    if (ai1->angle > ai2->angle) return 1;
    return 0;
}

MYIDEF vertices_t polygon_generate(vidx_t num) {
    coord_t minx = REAL_MAX_VALUE(minx),
            miny = REAL_MAX_VALUE(miny);
    coord_t maxx = -REAL_MAX_VALUE(maxx),
            maxy = -REAL_MAX_VALUE(maxy);
    vertices_t vertices = vertices_allocate(num);
    for (int i=0; i<num; ++i) {
        coord_t tx = (coord_t) rand() / RAND_MAX * 100;
        coord_t ty = (coord_t) rand() / RAND_MAX * 100;
        vertices_nth_setxy(vertices, i, tx, ty);
        if (tx < minx) minx = tx;
        if (tx > maxx) maxx = tx;
        if (ty < miny) miny = ty;
        if (ty > maxy) maxy = ty;
 //       DBG("tx: %.8f, ty: %.8f", tx,ty);
    }

    coord_t cx = minx + (maxx - minx) / 2;
    coord_t cy = miny + (maxy - miny) / 2;
    struct angle_ix *angles = calloc(num, sizeof(angles[0]));
    for (int i=0; i<vertices_num(vertices); ++i) {
        coord_t x1 = vertices_nth_getx(vertices, i);
        coord_t y1 = vertices_nth_gety(vertices, i);
        angles[i].i = i;
        angles[i].angle = angle_degree(x1, y1, cx, cy, cx, cy + 1.0);
    }
 //   INFO("minx=%.8f, miny=%.8f, maxx=%.8f, maxy=%.8f", minx,miny,maxx,maxy);
 //   INFO("c: %.8f, %.8f", cx, cy);
    qsort(angles, num, sizeof(angles[0]), angle_comp);

    vertices_t res = vertices_allocate(num);
    for (int i=0; i<vertices_num(vertices); ++i) {
        coord_t tx = vertices_nth_getx(vertices, angles[i].i);
        coord_t ty = vertices_nth_gety(vertices, angles[i].i);
        vertices_nth_setxy(res, i, tx, ty);
//        DBG("idx-angle: %d = %.4f vertex: (%f, %f)", angles[i].i, angles[i].angle, tx,ty);
    }
    free(angles);
    vertices_destroy(vertices);

    return res;
}

#endif // POLYGON_GENERATOR_IMPLEMENTAION
