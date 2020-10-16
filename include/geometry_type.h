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

MYIDEF coord_t polygon_area(const coord_seq_t* cs);
MYIDEF coord_t triangle_area(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb, const coord_t xc, const coord_t yc);

MYIDEF coord_t angle_degree(const coord_t x1, const coord_t y1, const coord_t x2, const coord_t y2, const coord_t x3, const coord_t y3);

MYIDEF bool between(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb, const coord_t xc, const coord_t yc);
MYIDEF bool intersects(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb,
                       const coord_t xc, const coord_t yc, const coord_t xd, const coord_t yd);

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
MYIDEF coord_t polygon_area(const coord_seq_t* cs)
{
    coord_t area = (__typeof__(area))0;
    for (vidx_t i = 0, im1 = cs->n - 1; i < cs->n; i++ ) {
        __auto_type x_im1 = coord_seq_getx(cs, im1);
        __auto_type y_im1 = coord_seq_gety(cs, im1);
        __auto_type x_i = coord_seq_getx(cs, i);
        __auto_type y_i = coord_seq_gety(cs, i);
        area += x_im1 * y_i - x_i * y_im1;
        im1 = i;
    }
    area = 0.5 * area;

    return area;
}

/**
  Purpose:
    TRIANGLE_AREA computes the signed area of a triangle.

  Modified:
    05 May 2014

  Author:
    John Burkardt

  Parameters:
    Input, double XA, YA, XB, YB, XC, YC, the coordinates of
    the vertices of the triangle, given in counterclockwise order.

    Output, double TRIANGLE_AREA, the signed area of the triangle.
*/
MYIDEF coord_t triangle_area(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb, const coord_t xc, const coord_t yc)
{
    return 0.5 * ( 
          ( xb - xa ) * ( yc - ya ) 
        - ( xc - xa ) * ( yb - ya ) );
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

#define THE_MIN(a, b) ((a) < (b) ? (a) : (b))
#define THE_MAX(a, b) ((a) > (b) ? (a) : (b))
#define THE_ABS(expr) ((expr) >= (__typeof__(expr))0 ? (expr) : -(expr))

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
static bool collinear(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb, const coord_t xc, const coord_t yc)
{
    __auto_type side_ab_sq = (xa - xb) * (xa - xb) + (ya - yb) * (ya - yb);
    __auto_type side_bc_sq = (xb - xc) * (xb - xc) + (yb - yc) * (yb - yc);
    __auto_type side_ca_sq = (xc - xa) * (xc - xa) + (yc - ya) * (yc - ya);

    __auto_type side_max_sq = THE_MAX(side_ab_sq, THE_MAX(side_bc_sq, side_ca_sq));

    if ( side_max_sq <= r8_eps ) {
        return true;
    }
    else if ( 2.0 * THE_ABS(triangle_area(xa, ya, xb, yb, xc, yc)) <= r8_eps * side_max_sq) {
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
MYIDEF bool between(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb, const coord_t xc, const coord_t yc)
{
    if ( ! collinear( xa, ya, xb, yb, xc, yc ) ) {
        return false;
    }

    if ( THE_ABS ( ya - yb ) < THE_ABS ( xa - xb ) ) {
        __auto_type xmax = THE_MAX ( xa, xb );
        __auto_type xmin = THE_MIN ( xa, xb );
        return ( xmin <= xc && xc <= xmax );
    }
    else {
        __auto_type ymax = THE_MAX ( ya, yb );
        __auto_type ymin = THE_MAX ( ya, yb );
        return ( ymin <= yc && yc <= ymax );
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
static bool intersect_prop(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb,
                    const coord_t xc, const coord_t yc, const coord_t xd, const coord_t yd)
{
    if ( collinear ( xa, ya, xb, yb, xc, yc ) ) {
        return false;
    }
    else if ( collinear ( xa, ya, xb, yb, xd, yd ) ) {
        return false;
    }
    else if ( collinear ( xc, yc, xd, yd, xa, ya ) ) {
        return false;
    }
    else if ( collinear ( xc, yc, xd, yd, xb, yb ) ) {
        return false;
    }
    else {
        __auto_type t1 = triangle_area ( xa, ya, xb, yb, xc, yc );
        __auto_type t2 = triangle_area ( xa, ya, xb, yb, xd, yd );
        __auto_type t3 = triangle_area ( xc, yc, xd, yd, xa, ya );
        __auto_type t4 = triangle_area ( xc, yc, xd, yd, xb, yb );

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
MYIDEF bool intersects(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb,
                       const coord_t xc, const coord_t yc, const coord_t xd, const coord_t yd)
{
    if ( intersect_prop ( xa, ya, xb, yb, xc, yc, xd, yd ) ) {
        return true;
    }
    else if ( between( xa, ya, xb, yb, xc, yc ) ) {
        return true;
    }
    else if ( between( xa, ya, xb, yb, xd, yd ) ) {
        return true;
    }
    else if ( between ( xc, yc, xd, yd, xa, ya ) ) {
        return true;
    }
    else if ( between ( xc, yc, xd, yd, xb, yb ) ) {
        return true;
    }
    else {
        return false;
    }
}



#endif // GEOM_TYPE_IMPLEMENTATION
