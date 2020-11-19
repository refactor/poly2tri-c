#ifndef POLY2TRI_INCLUDE_GEOMETRY_TYPE_H
#define POLY2TRI_INCLUDE_GEOMETRY_TYPE_H

#ifndef MYIDEF
#define MYIDEF extern
#else
#define MYIDEF
#endif

#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdalign.h>
#include <stdlib.h>

#ifdef USING_DOUBLE_COORD
typedef double coord_t;
#else
typedef float coord_t;
#endif

#ifdef USING_INT16_INDEX
typedef int16_t vidx_t;
#else
typedef int32_t vidx_t;
#endif

#define THE_MIN(a, b) ((a) < (b) ? (a) : (b))
#define THE_MAX(a, b) ((a) > (b) ? (a) : (b))
#define THE_ABS(expr) ({__auto_type v = (expr); v >= (__typeof__(v))0 ? v : -v;})

#define REAL_MAX_VALUE(x) _Generic((x), float:FLT_MAX, double: DBL_MAX) 

typedef struct triangles_s* triangles_t;

MYIDEF triangles_t triangles_allocate(vidx_t m);
MYIDEF void        triangles_free(triangles_t triangles);

MYIDEF vidx_t  triangles_num(triangles_t triangles);
MYIDEF vidx_t* triangles_nth(triangles_t triangles, vidx_t i);
MYIDEF vidx_t  triangles_append(triangles_t triangles, vidx_t a, vidx_t b, vidx_t c);


typedef struct vertices_s* vertices_t;

//MYIDEF vertices_t vertices_create(vidx_t n, const coord_t x[n], const coord_t y[n]);
MYIDEF vertices_t vertices_clone_floats(vidx_t n, const float x[n], const float y[n]);
MYIDEF vertices_t vertices_clone_doubles(vidx_t n, const double x[n], const double y[n]);
#define vertices_create(n, x, y) _Generic((x), float*:vertices_clone_floats, double*:vertices_clone_doubles)(n,x,y)

MYIDEF void       vertices_destroy(vertices_t poly);

MYIDEF vidx_t  vertices_num(const vertices_t poly);
MYIDEF coord_t vertices_nth_getx(const vertices_t cs, vidx_t idx);
MYIDEF coord_t vertices_nth_gety(const vertices_t cs, vidx_t idx);
MYIDEF void    vertices_nth_setxy(vertices_t cs, vidx_t idx, coord_t x, coord_t y);


typedef struct holes_s* holes_t;

//MYIDEF holes_t holes_create(vidx_t num, vidx_t holeIndices[num]);
MYIDEF holes_t holes_create_int16(int16_t num, int16_t holeIndices[num]);
MYIDEF holes_t holes_create_int32(int32_t num, int32_t holeIndices[num]);
#define holes_create(num, holeIndices) _Generic((num), int16_t:holes_create_int16, int32_t:holes_create_int32)(num, holeIndices)

MYIDEF void    holes_destory(holes_t holes);

MYIDEF vidx_t holes_num(holes_t holes);


typedef struct polygon_s* polygon_t;
MYIDEF polygon_t polygon_build(const vertices_t vertices, const holes_t holes);
MYIDEF void polygon_destroy(polygon_t polygon);

MYIDEF coord_t polygon_area(polygon_t polygon);
MYIDEF vertices_t polygon_getvertices(polygon_t polygon);

MYIDEF coord_t signed_area(const vertices_t cs, vidx_t start, vidx_t end);
MYIDEF coord_t triangle_area(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb, const coord_t xc, const coord_t yc);

MYIDEF coord_t angle_degree(const coord_t x1, const coord_t y1, const coord_t x2, const coord_t y2, const coord_t x3, const coord_t y3);

MYIDEF bool between(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb, const coord_t xc, const coord_t yc);
MYIDEF bool intersects(const coord_t xa, const coord_t ya, const coord_t xb, const coord_t yb,
                       const coord_t xc, const coord_t yc, const coord_t xd, const coord_t yd);

#endif // POLY2TRI_INCLUDE_GEOMETRY_TYPE_H

#ifdef GEOM_TYPE_IMPLEMENTATION

#define COORD_X_SZ sizeof(coord_t)
#define COORD_Y_SZ sizeof(coord_t)

struct vertices_s {
    union {
        const vidx_t n;
        vidx_t N;
    };
    alignas(8) coord_t vertices[];
};

struct holes_s {
    vidx_t num;
    alignas(8) vidx_t holeIndices[];
};

struct triangles_s {
    vidx_t m;
    alignas(8) vidx_t vidx[];
};

struct polygon_s {
    vertices_t vertices;
    holes_t holes;
};

MYIDEF polygon_t polygon_build(const vertices_t vertices, const holes_t holes) {
    polygon_t polygon = (__typeof__(polygon)) malloc(sizeof(*polygon));
    polygon->vertices = vertices;
    polygon->holes = holes;
    return polygon;
}

MYIDEF void polygon_destroy(polygon_t polygon) {
    if (polygon->vertices) vertices_destroy(polygon->vertices);
    if (polygon->holes) holes_destory(polygon->holes);
    free(polygon);
}

MYIDEF coord_t polygon_area(polygon_t polygon) {
    holes_t holes = polygon->holes;
    bool hasHole = holes != NULL;
    
    vidx_t holeStart = hasHole ? holes->holeIndices[0] : vertices_num(polygon->vertices);
    __auto_type sarea = THE_ABS(signed_area(polygon->vertices, 0, holeStart));
    __auto_type sum = (__typeof__(sarea))0;
    if (hasHole) {
        for (vidx_t i = 0; i < holes->num; ++i) {
            vidx_t start = holes->holeIndices[i];
            vidx_t end = i < holes->num - 1 ? holes->holeIndices[i + 1] : vertices_num(polygon->vertices);
            sum += THE_ABS(signed_area(polygon->vertices, start, end));
        }
    }
    return sarea - sum;
}

MYIDEF vertices_t polygon_getvertices(polygon_t polygon) {
    return polygon->vertices;
}

MYIDEF triangles_t triangles_allocate(vidx_t m) {
    triangles_t triangles =
        (__typeof__(triangles)) aligned_alloc(8, sizeof(*triangles) + m * 3 * sizeof(vidx_t));
    triangles->m = 0;
    return triangles;
}

MYIDEF void triangles_free(triangles_t triangles) {
    free(triangles);
}

MYIDEF vidx_t triangles_num(triangles_t triangles) {
    return triangles->m;
}

MYIDEF vidx_t* triangles_nth(triangles_t triangles, vidx_t i) {
    return &triangles->vidx[i * 3];
}

MYIDEF vidx_t triangles_append(triangles_t triangles, vidx_t a, vidx_t b, vidx_t c) {
    vidx_t i = triangles->m;
    __auto_type tri = triangles_nth(triangles, i);
    tri[0] = a;
    tri[1] = b;
    tri[2] = c;
    triangles->m = i + 1;
    return triangles->m;
}

vertices_t vertices_allocate(vidx_t n) {
    vertices_t cs = (__typeof__(cs)) aligned_alloc(8, sizeof(*cs) + n * (COORD_X_SZ + COORD_Y_SZ) );
    cs->N = n;
    return cs;
}

MYIDEF vertices_t vertices_clone_doubles(vidx_t n, const double x[n], const double y[n]) {
    vertices_t polygon = vertices_allocate(n);
    for (__auto_type i=0; i<n; ++i) {
        vertices_nth_setxy(polygon, i, (coord_t)x[i], (coord_t)y[i]);
    }
    return polygon;
}

MYIDEF vertices_t vertices_clone_floats(vidx_t n, const float x[n], const float y[n]) {
    vertices_t polygon = vertices_allocate(n);
    for (__auto_type i=0; i<n; ++i) {
        vertices_nth_setxy(polygon, i, (coord_t)x[i], (coord_t)y[i]);
    }
    return polygon;
}

MYIDEF void vertices_destroy(vertices_t poly) {
    free(poly);
}

MYIDEF vidx_t vertices_num(const vertices_t poly) {
    return poly->n;
}

MYIDEF coord_t vertices_nth_getx(const vertices_t cs, vidx_t idx) {
    return cs->vertices[idx];
}

MYIDEF void vertices_nth_setxy(vertices_t cs, vidx_t idx, coord_t x, coord_t y) {
    cs->vertices[idx] = x;
    cs->vertices[cs->n + idx] = y;
}

MYIDEF coord_t vertices_nth_gety(const vertices_t cs, vidx_t idx) {
    return cs->vertices[cs->n  + idx];
}

MYIDEF holes_t holes_create_int16(int16_t num, int16_t holeIndices[num]) {
//MYIDEF holes_t holes_create(vidx_t num, vidx_t holeIndices[num]) {
    holes_t holes = (__typeof__(holes)) aligned_alloc(8, sizeof(*holes) + num * sizeof(holes->holeIndices[0]));
    holes->num = num;
    for (vidx_t i = 0; i < num; ++i) holes->holeIndices[i] = holeIndices[i];
    return holes;
}
MYIDEF holes_t holes_create_int32(int32_t num, int32_t holeIndices[num]) {
    holes_t holes = (__typeof__(holes)) aligned_alloc(8, sizeof(*holes) + num * sizeof(holes->holeIndices[0]));
    holes->num = num;
    for (vidx_t i = 0; i < num; ++i) holes->holeIndices[i] = holeIndices[i];
    return holes;
}

MYIDEF void holes_destory(holes_t holes) {
    free(holes);
}

MYIDEF vidx_t holes_num(holes_t holes) {
    return holes->num;
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
MYIDEF coord_t signed_area(const vertices_t cs, vidx_t start, vidx_t end)
{
    coord_t area = (__typeof__(area))0;
    for (vidx_t i = start, j = end - 1; i < end; i++ ) {
        __auto_type x_j = vertices_nth_getx(cs, j);
        __auto_type y_j = vertices_nth_gety(cs, j);
        __auto_type x_i = vertices_nth_getx(cs, i);
        __auto_type y_i = vertices_nth_gety(cs, i);
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
