#ifndef POLY2TRI_INCLUDE_GEOMETRY_TYPE_H
#define POLY2TRI_INCLUDE_GEOMETRY_TYPE_H

#ifndef MYIDEF
#ifdef MY_GEOM_TYPE_STATIC
#define MYIDEF static
#else
#define MYIDEF extern
#endif
#endif

#include <stdbool.h>
#include <inttypes.h>
#include <stdalign.h>
#include <stdlib.h>

#ifdef USING_DOUBLE
typedef double coord_t;
typedef uint32_t vidx_t;
#else
typedef float coord_t;
typedef uint16_t vidx_t;
#endif

typedef struct coord_seq_t {
    vidx_t n;
    alignas(8) coord_t vertices[];
} coord_seq_t;

MYIDEF coord_seq_t* allocate_coord_seq(vidx_t n);
MYIDEF coord_t coord_seq_getx(const coord_seq_t* cs, vidx_t idx);
MYIDEF void coord_seq_setx(coord_seq_t* cs, vidx_t idx, coord_t x);
MYIDEF coord_t coord_seq_gety(const coord_seq_t* cs, vidx_t idx);
MYIDEF void coord_seq_sety(coord_seq_t* cs, vidx_t idx, coord_t y);

#endif // POLY2TRI_INCLUDE_GEOMETRY_TYPE_H

#ifdef GEOM_TYPE_IMPLEMENTATION

#define COORD_X_SZ sizeof(coord_t)
#define COORD_Y_SZ sizeof(coord_t)

MYIDEF coord_seq_t* allocate_coord_seq(vidx_t n) {
    coord_seq_t *cs = (__typeof__(cs)) aligned_alloc(8, sizeof(*cs) + n * (COORD_X_SZ + COORD_Y_SZ) );
    cs->n = n;
    return cs;
}

MYIDEF coord_t coord_seq_getx(const coord_seq_t* cs, vidx_t idx) {
    return cs->vertices[idx];
}

MYIDEF void coord_seq_setx(coord_seq_t* cs, vidx_t idx, coord_t x) {
    cs->vertices[idx] = x;
}

MYIDEF coord_t coord_seq_gety(const coord_seq_t* cs, vidx_t idx) {
    return cs->vertices[cs->n  + idx];
}

MYIDEF void coord_seq_sety(coord_seq_t* cs, vidx_t idx, coord_t y) {
    cs->vertices[cs->n + idx] = y;
}


#endif // GEOM_TYPE_IMPLEMENTATION
