#ifndef POLY2TRI_INCLUDE_GEOMETRY_TYPE_H
#define POLY2TRI_INCLUDE_GEOMETRY_TYPE_H

#ifndef MYIDEF
#define MYIDEF extern
#else
#define MYIDEF
#endif

# include <math.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdalign.h>
#include <stdlib.h>

#ifdef USING_DOUBLE_COORD
typedef double coord_t;
#else
typedef float coord_t;
#endif

#ifdef USING_INT32_INDEX
typedef int32_t vidx_t;
#else
typedef int16_t vidx_t;
#endif

#define THE_MIN(a, b) ((a) < (b) ? (a) : (b))
#define THE_MAX(a, b) ((a) > (b) ? (a) : (b))
#define THE_ABS(expr) ((expr) >= (__typeof__(expr))0 ? (expr) : -(expr))

typedef struct polygon_s * polygon_t;

typedef struct triangles_s {
    vidx_t m;
    alignas(8) vidx_t vidx[];
} triangles_t;

MYIDEF triangles_t* allocate_triangles(vidx_t m);
MYIDEF polygon_t allocate_polygon(vidx_t n);
MYIDEF void free_polygon(polygon_t poly);

MYIDEF vidx_t vertices_num(const polygon_t poly);
MYIDEF coord_t coord_seq_getx(const polygon_t cs, vidx_t idx);
MYIDEF void coord_seq_setx(polygon_t cs, vidx_t idx, coord_t x);
MYIDEF coord_t coord_seq_gety(const polygon_t cs, vidx_t idx);
MYIDEF void coord_seq_sety(polygon_t cs, vidx_t idx, coord_t y);

MYIDEF coord_t signed_area(const polygon_t cs);
MYIDEF coord_t triangle_area(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb, const coord_t xc, const coord_t yc);

MYIDEF coord_t angle_degree(const coord_t x1, const coord_t y1, const coord_t x2, const coord_t y2, const coord_t x3, const coord_t y3);

MYIDEF bool between(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb, const coord_t xc, const coord_t yc);
MYIDEF bool intersects(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb,
                       const coord_t xc, const coord_t yc, const coord_t xd, const coord_t yd);

#endif // POLY2TRI_INCLUDE_GEOMETRY_TYPE_H

#ifdef GEOM_TYPE_IMPLEMENTATION

#define COORD_X_SZ sizeof(coord_t)
#define COORD_Y_SZ sizeof(coord_t)

struct polygon_s {
    union {
        const vidx_t n;
        vidx_t N;
    };
    alignas(8) coord_t vertices[];
};

MYIDEF triangles_t* allocate_triangles(vidx_t m) {
    triangles_t* triangles =
        (__typeof__(triangles)) aligned_alloc(8, sizeof(*triangles) + m * 3 * sizeof(vidx_t));
    triangles->m = 0;
    return triangles;
}

MYIDEF polygon_t allocate_polygon(vidx_t n) {
    polygon_t cs = (__typeof__(cs)) aligned_alloc(8, sizeof(*cs) + n * (COORD_X_SZ + COORD_Y_SZ) );
    cs->N = n;
    return cs;
}

MYIDEF void free_polygon(polygon_t poly) {
    free(poly);
}

MYIDEF vidx_t vertices_num(const polygon_t poly) {
    return poly->n;
}

MYIDEF coord_t coord_seq_getx(const polygon_t cs, vidx_t idx) {
    return cs->vertices[idx];
}

MYIDEF void coord_seq_setx(polygon_t cs, vidx_t idx, coord_t x) {
    cs->vertices[idx] = x;
}

MYIDEF coord_t coord_seq_gety(const polygon_t cs, vidx_t idx) {
    return cs->vertices[cs->n  + idx];
}

MYIDEF void coord_seq_sety(polygon_t cs, vidx_t idx, coord_t y) {
    cs->vertices[cs->n + idx] = y;
}

/**
  Purpose:
    POLYGON_AREA returns the area of a polygon.

  Discussion:
    The vertices should be listed in counter-clockwise order so that
    the area will be positive.

  Modified:
    10 September 2016

  Author:
    John Burkardt.

  Parameters:
    Input, int N, the number of vertices.
    Input, double X[N], Y[N], the vertex coordinates.
    Output, double POLYGON_AREA, the area of the polygon.
*/
MYIDEF coord_t signed_area(const polygon_t cs)
{
    coord_t area = (__typeof__(area))0;
    for (vidx_t i = 0, j = cs->n - 1; i < cs->n; i++ ) {
        __auto_type x_j = coord_seq_getx(cs, j);
        __auto_type y_j = coord_seq_gety(cs, j);
        __auto_type x_i = coord_seq_getx(cs, i);
        __auto_type y_i = coord_seq_gety(cs, i);
        area += (x_j - x_i) * (y_i + y_j);
//        area += x_j * y_i - x_i * y_j;
        j = i;
    }
    //area = 0.5 * area; // it doesn't mater

    return area;
}

/**
  Purpose:
    TRIANGLE_AREA computes the signed area of a triangle.
    The signed area of a triangle is just the area of a triangle, if the vertices are listed counterclockwise,
    or negative of that area, if the vertices are listed clockwise.

  Modified:
    05 May 2014

  Author:
    John Burkardt

  Parameters:
    Input, double XA, YA, XB, YB, XC, YC, the coordinates of
    the vertices of the triangle, given in counterclockwise order.

    Output, double TRIANGLE_AREA, the signed area of the triangle.
*/
MYIDEF coord_t triangle_area(const coord_t ax, const coord_t ay, const coord_t bx, const coord_t by, const coord_t cx, const coord_t cy)
{
    return /*0.5 * */( 
          ( bx - ax ) * ( cy - ay ) 
        - ( cx - ax ) * ( by - ay ) );
}

#define THE_ATAN2(y,x) _Generic((y), float:atan2f(y,x), double:atan2(y,x))

/**
  Purpose:
    ANGLE_DEGREE returns the degree angle defined by three points.

  Discussion:

        P1
        /
       /
      /
     /
    P2--------->P3

  Modified:
    28 August 2016

  Author:
    John Burkardt

  Parameters:
    Input, double X1, Y1, X2, Y2, X3, Y3, the coordinates of the points P1, P2, P3.

    Output, double ANGLE_DEGREE, the angle swept out by the rays, measured
    in degrees.  0 <= VALUE < 360.  If either ray has zero length,
    then VALUE is set to 0.
*/
#define r8_pi 3.141592653589793
MYIDEF coord_t angle_degree(const coord_t x1, const coord_t y1, const coord_t x2, const coord_t y2, const coord_t x3, const coord_t y3 )
{

    __auto_type x = ( x3 - x2 ) * ( x1 - x2 ) + ( y3 - y2 ) * ( y1 - y2 );
    __auto_type y = ( x3 - x2 ) * ( y1 - y2 ) - ( y3 - y2 ) * ( x1 - x2 );

    if ( x == 0.0 && y == 0.0 ) {
        return 0.0;
    }
    else {
        __auto_type value = THE_ATAN2( y, x );
        if ( value < 0.0 ) {
            value += 2.0 * r8_pi;
        }
        return 180.0 * value / r8_pi;
    }
}

/**
  Purpose:
    COLLINEAR returns a measure of collinearity for three points.

  Discussion:
    In order to deal with collinear points whose coordinates are not
    numerically exact, we compare the area of the largest square
    that can be created by the line segment between two of the points
    to (twice) the area of the triangle formed by the points.

    If the points are collinear, their triangle has zero area.
    If the points are close to collinear, then the area of this triangle
    will be small relative to the square of the longest segment.

  Modified:
    10 September 2016

  Author:
    Original C version by Joseph ORourke.
    This C version by John Burkardt.

  Reference:
    Joseph ORourke,
    Computational Geometry in C,
    Cambridge, 1998,
    ISBN: 0521649765,
    LC: QA448.D38.

  Parameters:
    Input, double XA, YA, XB, YB, XC, YC, the coordinates of 
    the vertices.

    Output, int COLLINEAR, is TRUE if the points are judged 
    to be collinear.
*/
#define r8_eps 2.220446049250313E-016
static bool collinear(const coord_t ax, const coord_t ay, const coord_t bx, const coord_t by, const coord_t cx, const coord_t cy)
{
    __auto_type side_ab_sq = (ax - bx) * (ax - bx) + (ay - by) * (ay - by);
    __auto_type side_bc_sq = (bx - cx) * (bx - cx) + (by - cy) * (by - cy);
    __auto_type side_ca_sq = (cx - ax) * (cx - ax) + (cy - ay) * (cy - ay);

    __auto_type side_max_sq = THE_MAX(side_ab_sq, THE_MAX(side_bc_sq, side_ca_sq));

    if ( side_max_sq <= r8_eps ) {
        return true;
    }
    else if ( 2.0 * THE_ABS(triangle_area(ax, ay, bx, by, cx, cy)) <= r8_eps * side_max_sq) {
        return true;
    }

    return false;
}

/**
  Purpose:
    BETWEEN is TRUE if vertex C is between vertices A and B.

  Discussion:
    The points must be (numerically) collinear.

    Given that condition, we take the greater of XA - XB and YA - YB
    as a "scale" and check where C's value lies.

  Modified:
    05 May 2014

  Author:
    Original C version by Joseph ORourke.
    This C version by John Burkardt.

  Reference:
    Joseph ORourke,
    Computational Geometry in C,
    Cambridge, 1998,
    ISBN: 0521649765,
    LC: QA448.D38.

  Parameters:
    Input, double XA, YA, XB, YB, XC, YC, the coordinates of 
    the vertices.

    Output, int BETWEEN, is TRUE if C is between A and B.
*/
MYIDEF bool between(const coord_t ax, const coord_t ay, const coord_t bx, const coord_t by, const coord_t cx, const coord_t cy)
{
    if ( ! collinear( ax, ay, bx, by, cx, cy ) ) {
        return false;
    }

    if ( THE_ABS ( ay - by ) < THE_ABS ( ax - bx ) ) {
        __auto_type xmax = THE_MAX ( ax, bx );
        __auto_type xmin = THE_MIN ( ax, bx );
        return ( xmin <= cx && cx <= xmax );
    }
    else {
        __auto_type ymax = THE_MAX ( ay, by );
        __auto_type ymin = THE_MAX ( ay, by );
        return ( ymin <= cy && cy <= ymax );
    }
}

/**
  Purpose:
    L4_XOR returns the exclusive OR of two L4's.

  Discussion:
    An L4 is a logical value.

  Modified:
    04 May 2014

  Author:
   John Burkardt

  Parameters:
    Input, int L1, L2, two values whose exclusive OR is needed.
    Output, int L4_XOR, the exclusive OR of L1 and L2.
*/
static bool l4_xor(bool l1, bool l2)
{
    bool value1 = (     l1   && ( ! l2 ) );
    bool value2 = ( ( ! l1 ) &&     l2   );

    return ( value1 || value2 );
}

/**
  Purpose:
    INTERSECT_PROP is TRUE if lines VA:VB and VC:VD have a proper intersection.

  Licensing:
    This code is distributed under the GNU LGPL license.

  Modified:
    04 May 2014

  Author:
    Original C version by Joseph ORourke.
    This C version by John Burkardt.

  Reference:
    Joseph ORourke,
    Computational Geometry in C,
    Cambridge, 1998,
    ISBN: 0521649765,
    LC: QA448.D38.

  Parameters:
    Input, double XA, YA, XB, YB, XC, YC, XD, YD, the X and Y coordinates of the four vertices.
    Output, int INTERSECT_PROP, the result of the test.
*/
static bool intersect_prop(const coord_t ax, const coord_t ay, const coord_t bx, const coord_t by,
                           const coord_t cx, const coord_t cy, const coord_t dx, const coord_t dy)
{
    if ( collinear ( ax, ay, bx, by, cx, cy ) ) {
        return false;
    }
    else if ( collinear ( ax, ay, bx, by, dx, dy ) ) {
        return false;
    }
    else if ( collinear ( cx, cy, dx, dy, ax, ay ) ) {
        return false;
    }
    else if ( collinear ( cx, cy, dx, dy, bx, by ) ) {
        return false;
    }
    else {
        __auto_type t1 = triangle_area ( ax, ay, bx, by, cx, cy );
        __auto_type t2 = triangle_area ( ax, ay, bx, by, dx, dy );
        __auto_type t3 = triangle_area ( cx, cy, dx, dy, ax, ay );
        __auto_type t4 = triangle_area ( cx, cy, dx, dy, bx, by );

        bool value1 = ( 0.0 < t1 );
        bool value2 = ( 0.0 < t2 );
        bool value3 = ( 0.0 < t3 );
        bool value4 = ( 0.0 < t4 );
        return ( l4_xor ( value1, value2 ) ) && ( l4_xor ( value3, value4 ) );
    }
}

/**
  Purpose:
    INTERSECT is true if lines VA:VB and VC:VD intersects.

  Discussion:
    Thanks to Gene Dial for correcting the call to intersect_prop, 
    08 September 2016.
 
  Modified:
    08 September 2016

  Author:
    Original C version by Joseph ORourke.
    This C version by John Burkardt.

  Reference:
    Joseph ORourke,
    Computational Geometry in C,
    Cambridge, 1998,
    ISBN: 0521649765,
    LC: QA448.D38.

  Parameters:
    Input, double XA, YA, XB, YB, XC, YC, XD, YD, the X and Y 
    coordinates of the four vertices.

    Output, int INTERSECT, the value of the test.
*/
MYIDEF bool intersects(const coord_t ax, const coord_t ay, const coord_t bx, const coord_t by,
                       const coord_t cx, const coord_t cy, const coord_t dx, const coord_t dy)
{
    if ( intersect_prop ( ax, ay, bx, by, cx, cy, dx, dy ) ) {
        return true;
    }
    else if ( between( ax, ay, bx, by, cx, cy ) ) {
        return true;
    }
    else if ( between( ax, ay, bx, by, dx, dy ) ) {
        return true;
    }
    else if ( between ( cx, cy, dx, dy, ax, ay ) ) {
        return true;
    }
    else if ( between ( cx, cy, dx, dy, bx, by ) ) {
        return true;
    }
    else {
        return false;
    }
}


#endif // GEOM_TYPE_IMPLEMENTATION
