#ifndef POLY2TRI_INCLUDE_H
#define POLY2TRI_INCLUDE_H

#include "geometry_type.h"

vidx_t *polygon_triangulate(const coord_seq_t* cs);

coord_t polygon_area(const coord_seq_t* cs);
coord_t angle_degree(const coord_t x1, const coord_t y1, const coord_t x2, const coord_t y2, const coord_t x3, const coord_t y3);
coord_t triangle_area(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb, const coord_t xc, const coord_t yc);

bool between(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb, const coord_t xc, const coord_t yc);
bool collinear(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb, const coord_t xc, const coord_t yc);
bool intersects(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb, const coord_t xc, const coord_t yc, const coord_t xd, const coord_t yd);

#endif // POLY2TRI_INCLUDE_H


//#ifdef POLY2TRI_IMPLEMENTATION

