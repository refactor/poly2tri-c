# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>

# include "polygon_triangulate.h"
# include "fmt_utils.h"
# include "mylog.h"

#define THE_MIN(a, b) ((a) < (b) ? (a) : (b))
#define THE_MAX(a, b) ((a) > (b) ? (a) : (b))

#define THE_ABS(expr) ((expr) >= (__typeof__(expr))0 ? (expr) : -(expr))

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
coord_t angle_degree(coord_t x1, coord_t y1, coord_t x2, coord_t y2, coord_t x3, coord_t y3 )
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
bool between(coord_t xa, coord_t ya, coord_t xb, coord_t yb, coord_t xc, coord_t yc)
{
    if ( ! collinear( xa, ya, xb, yb, xc, yc ) ) {
        return false;
    }

    __typeof__(xa) xmax;
    __typeof__(xa) xmin;
    __typeof__(ya) ymax;
    __typeof__(ya) ymin;
    if ( THE_ABS ( ya - yb ) < THE_ABS ( xa - xb ) ) {
        xmax = THE_MAX ( xa, xb );
        xmin = THE_MIN ( xa, xb );
        return ( xmin <= xc && xc <= xmax );
    }
    else {
        ymax = THE_MAX ( ya, yb );
        ymin = THE_MAX ( ya, yb );
        return ( ymin <= yc && yc <= ymax );
    }
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
coord_t triangle_area(coord_t xa, coord_t ya, coord_t xb, coord_t yb, coord_t xc, coord_t yc)
{
    return 0.5 * ( 
          ( xb - xa ) * ( yc - ya ) 
        - ( xc - xa ) * ( yb - ya ) );
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
bool collinear(coord_t xa, coord_t ya, coord_t xb, coord_t yb, coord_t xc, coord_t yc)
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
    IN_CONE is TRUE if the diagonal VERTEX(IM1):VERTEX(IP1) is strictly internal.

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
    Input, IM1, IP1, the indices of two vertices.
    Input, PREV_NODE[N], the previous neighbor of each vertex.
    Input, NEXT_NODE[N], the next neighbor of each vertex.
    Input, double X[N], Y[N], the coordinates of each vertex.

    Output, int IN_CONE, the value of the test.
*/
bool in_cone(vidx_t im1, vidx_t ip1, vidx_t prev_node[], vidx_t next_node[], coord_t x[], coord_t y[])
{
    __auto_type im2 = prev_node[im1];
    __auto_type i   = next_node[im1];

    if (0.0 <= triangle_area(x[im1], y[im1], x[i], y[i], x[im2], y[im2])) {
        bool t2 = triangle_area(x[im1], y[im1], x[ip1], y[ip1], x[im2], y[im2]) > 0.0;
        bool t3 = triangle_area(x[ip1], y[ip1], x[im1], y[im1], x[i], y[i]) > 0.0;
        return ( t2 && t3 );
    }
    else {
        bool t4 = triangle_area(x[im1], y[im1], x[ip1], y[ip1], x[i], y[i]) >= 0.0;
        bool t5 = triangle_area(x[ip1], y[ip1], x[im1], y[im1], x[im2], y[im2]) >= 0.0;
        return ! ( t4 && t5 );
    }
}

/**
  Purpose:
    DIAGONALIE is true if VERTEX(IM1):VERTEX(IP1) is a proper diagonal.

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

    Input, int IM1, IP1, the indices of two vertices.

    Input, int N, the number of vertices.

    Input, int NEXT_NODE[N], the next neighbor of each vertex.

    Input, double X[N], Y[N], the coordinates of each vertex.

    Output, int DIAGONALIE, the value of the test.
*/
bool diagonalie(vidx_t im1, vidx_t ip1, vidx_t next_node[], coord_t x[], coord_t y[])
{
    vidx_t first = im1;
    vidx_t j = first;
    vidx_t jp1 = next_node[first];

    /*
    For each edge VERTEX(J):VERTEX(JP1) of the polygon:
    */
    while ( true ) {
        /*
        Skip any edge that includes vertex IM1 or IP1.
        */
        if ( j == im1 || j == ip1 || jp1 == im1 || jp1 == ip1 ) {
        }
        else {
            if (intersects(x[im1], y[im1], x[ip1], y[ip1], x[j], y[j], x[jp1], y[jp1])) {
                return false;
            }
        }

        j = jp1;
        jp1 = next_node[j];

        if ( j == first ) {
            return true;
        }
    }
}

/**
  Purpose:
    DIAGONAL: VERTEX(IM1) to VERTEX(IP1) is a proper internal diagonal.

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

    Input, int IM1, IP1, the indices of two vertices.
    Input, int PREV_NODE[N], the previous neighbor of each vertex.
    Input, int NEXT_NODE[N], the next neighbor of each vertex.
    Input, double X[N], Y[N], the coordinates of each vertex.

    Output, int DIAGONAL, the value of the test.
*/
bool diagonal(vidx_t im1, vidx_t ip1, vidx_t prev_node[], vidx_t next_node[], coord_t x[], coord_t y[])
{
    bool value1 = in_cone(im1, ip1, prev_node, next_node, x, y);
    bool value2 = in_cone(ip1, im1, prev_node, next_node, x, y);
    bool value3 = diagonalie(im1, ip1, next_node, x, y);

    return ( value1 && value2 && value3 );
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
bool l4_xor(bool l1, bool l2)
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
bool intersect_prop(coord_t xa, coord_t ya, coord_t xb, coord_t yb, coord_t xc, coord_t yc, coord_t xd, coord_t yd)
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
bool intersects(coord_t xa, coord_t ya, coord_t xb, coord_t yb,
                coord_t xc, coord_t yc, coord_t xd, coord_t yd)
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
coord_t polygon_area(uint32_t n, coord_t x[n], coord_t y[n])
{
    coord_t area = (__typeof__(area))0;
    for (vidx_t i = 0, im1 = n - 1; i < n; i++ ) {
        area += x[im1] * y[i] - x[i] * y[im1];
        im1 = i;
    }
    area = 0.5 * area;

    return area;
}

/**
  Purpose:
    POLYGON_TRIANGULATE determines a triangulation of a polygon.

  Discussion:
    There are N-3 triangles in the triangulation.

    For the first N-2 triangles, the first edge listed is always an
    internal diagonal.

    Thanks to Gene Dial for pointing out a mistake in the area calculation,
    10 September 2016.

    Gene Dial requested an angle tolerance of about 1 millionth radian or 
    5.7E-05 degrees, 26 June 2018.

  Modified:
    26 June 2018

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
    Input, int N, the number of vertices.
    Input, double X[N], Y[N], the coordinates of each vertex.

    Output, int TRIANGLES[3*(N-2)], the triangles of the triangulation.
*/
#define angle_tol 5.7E-05
vidx_t *polygon_triangulate(const uint32_t n, coord_t x[n], coord_t y[n])
{

    // We must have at least 3 vertices.
    if (n < 3) {
        ERR("POLYGON_TRIANGULATE - Fatal error!  N < 3." );
        return NULL;
    }
    // Consecutive vertices cannot be equal.
    for (vidx_t current = 0, prev = n-1; current < n; current++ ) {
        if ( x[prev] == x[current] && y[prev] == y[current] ) {
          ERR("POLYGON_TRIANGULATE - Fatal error!  Two consecutive nodes are identical." );
          return NULL;
        }
        prev = current;
    }
    // No node can be the vertex of an angle less than 1 degree 
    // in absolute value.
    for (vidx_t current = 0, prev = n-1; current < n; current++ ) {
        vidx_t next = ( ( current + 1 ) % n ); 

        __auto_type angle = angle_degree(x[prev], y[prev], 
                                         x[current], y[current], 
                                         x[next], y[next] );

        if ( THE_ABS( angle ) <= angle_tol ) {
            ERR("POLYGON_TRIANGULATE - Fatal error! Polygon has an angle smaller than %g, accurring at node %d", angle_tol, current);
            return NULL;
        }
        prev = current;
    }
    // Area must be positive.
    if (polygon_area(n, x, y) <= 0.0) {
        ERR("POLYGON_TRIANGULATE - Fatal error!  Polygon has zero or negative area." );
        return NULL;
    }

    vidx_t* triangles = (__typeof__(triangles)) malloc ( 3 * ( n - 2 ) * sizeof ( *triangles ) );

    // PREV_NODE and NEXT_NODE point to the previous and next nodes.
    vidx_t* prev_node = (__typeof__(prev_node)) malloc ( n * sizeof ( *prev_node ) );
    vidx_t* next_node = (__typeof__(next_node)) malloc ( n * sizeof ( *next_node ) );

    for (vidx_t i = 0; i < (vidx_t)n; i++ ) {
        prev_node[i] = (i - 1 + n) % n;
        next_node[i] = (i + 1) % n;
    }

    // EAR indicates whether the node and its immediate neighbors form an ear
    // that can be sliced off immediately.
    bool* ear = (__typeof__(ear)) malloc ( n * sizeof ( *ear ) );
    for (vidx_t i = 0; i < (vidx_t)n; i++ ) {
        ear[i] = diagonal(prev_node[i], next_node[i], prev_node, next_node, x, y);
    }

    uint32_t triangle_idx = 0;

    vidx_t i0;
    vidx_t i1;
    vidx_t i2;
    vidx_t i3;
    vidx_t i4;

    i2 = 0;
    while (triangle_idx < n - 3) {
        // If I2 is an ear, gather information necessary to carry out
        // the slicing operation and subsequent "healing".
        if (ear[i2]) {
            i3 = next_node[i2];
            i4 = next_node[i3];
            i1 = prev_node[i2];
            i0 = prev_node[i1];
            // Make vertex I2 disappear.
            next_node[i1] = i3;
            prev_node[i3] = i1;
            // Update the earity of I1 and I3, because I2 disappeared.
            ear[i1] = diagonal ( i0, i3, prev_node, next_node, x, y );
            ear[i3] = diagonal ( i1, i4, prev_node, next_node, x, y );
            // Add the diagonal [I3, I1, I2] to the list.
            triangles[0+triangle_idx*3] = i3;
            triangles[1+triangle_idx*3] = i1;
            triangles[2+triangle_idx*3] = i2;
            ++triangle_idx;
        }
        // Try the next vertex.
        i2 = next_node[i2];
    }
    // The last triangle is formed from the three remaining vertices.
    i3 = next_node[i2];
    i1 = prev_node[i2];

    triangles[0+triangle_idx*3] = i3;
    triangles[1+triangle_idx*3] = i1;
    triangles[2+triangle_idx*3] = i2;

    free ( ear );
    free ( next_node );
    free ( prev_node );

    return triangles;
}
