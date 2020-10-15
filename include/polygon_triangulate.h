#pragma once

#include "geometry_type.h"

vidx_t* polygon_triangulate(uint32_t n, coord_t x[], coord_t y[]);

coord_t angle_degree(coord_t x1, coord_t y1, coord_t x2, coord_t y2, coord_t x3, coord_t y3);
coord_t polygon_area(uint32_t n, coord_t x[n], coord_t y[n]);
coord_t triangle_area(coord_t xa, coord_t ya, coord_t xb, coord_t yb, coord_t xc, coord_t yc );

bool between(coord_t xa, coord_t ya, coord_t xb, coord_t yb, coord_t xc, coord_t yc);
bool collinear(coord_t xa, coord_t ya, coord_t xb, coord_t yb, coord_t xc, coord_t yc);
bool intersects(coord_t xa, coord_t ya, coord_t xb, coord_t yb, coord_t xc, coord_t yc, coord_t xd, coord_t yd);

