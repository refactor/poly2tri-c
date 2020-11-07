/**
 * triangulate a possibly nonconvex polygon in 2D
 *
 * https://people.sc.fsu.edu/~jburkardt/c_src/polygon_triangulate/
 */
#ifndef POLY2TRI_INCLUDE_H
#define POLY2TRI_INCLUDE_H

#ifdef POLY2TRI_IMPLEMENTATION
#define GEOM_TYPE_IMPLEMENTATION
#endif
#include "geometry_type.h"

triangles_t *polygon_triangulate(const polygon_t* cs);

#endif // POLY2TRI_INCLUDE_H


//#ifdef POLY2TRI_IMPLEMENTATION

#ifdef POLY2TRI_IMPLEMENTATION

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>

#include "mylog.h"

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
static bool diagonalie(vidx_t im1, vidx_t ip1, vidx_t next_node[], const polygon_t* cs)
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
            __auto_type x_im1 = coord_seq_getx(cs, im1);
            __auto_type y_im1 = coord_seq_gety(cs, im1);
            __auto_type x_ip1 = coord_seq_getx(cs, ip1);
            __auto_type y_ip1 = coord_seq_gety(cs, ip1);
            __auto_type x_j = coord_seq_getx(cs, j);
            __auto_type y_j = coord_seq_gety(cs, j);
            __auto_type x_jp1 = coord_seq_getx(cs, jp1);
            __auto_type y_jp1 = coord_seq_gety(cs, jp1);
            if (intersects(x_im1, y_im1, x_ip1, y_ip1, x_j, y_j, x_jp1, y_jp1)) {
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
static bool in_cone(vidx_t im1, vidx_t ip1, vidx_t prev_node[], vidx_t next_node[], const polygon_t* cs)
{
    __auto_type im2 = prev_node[im1];
    __auto_type i   = next_node[im1];

    __auto_type x_im1 = coord_seq_getx(cs, im1);
    __auto_type y_im1 = coord_seq_gety(cs, im1);
    __auto_type x_im2 = coord_seq_getx(cs, im2);
    __auto_type y_im2 = coord_seq_gety(cs, im2);
    __auto_type x_ip1 = coord_seq_getx(cs, ip1);
    __auto_type y_ip1 = coord_seq_gety(cs, ip1);
    __auto_type x_i = coord_seq_getx(cs, i);
    __auto_type y_i = coord_seq_gety(cs, i);
    if (0.0 <= triangle_area(x_im1, y_im1, x_i, y_i, x_im2, y_im2)) {
        bool t2 = triangle_area(x_im1, y_im1, x_ip1, y_ip1, x_im2, y_im2) > 0.0;
        bool t3 = triangle_area(x_ip1, y_ip1, x_im1, y_im1, x_i, y_i) > 0.0;
        return ( t2 && t3 );
    }
    else {
        bool t4 = triangle_area(x_im1, y_im1, x_ip1, y_ip1, x_i, y_i) >= 0.0;
        bool t5 = triangle_area(x_ip1, y_ip1, x_im1, y_im1, x_im2, y_im2) >= 0.0;
        return ! ( t4 && t5 );
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
bool diagonal(vidx_t im1, vidx_t ip1, vidx_t prev_node[], vidx_t next_node[], const polygon_t* cs)
{
    bool value1 = in_cone(im1, ip1, prev_node, next_node, cs);
    bool value2 = in_cone(ip1, im1, prev_node, next_node, cs);
    bool value3 = diagonalie(im1, ip1, next_node, cs);

    return ( value1 && value2 && value3 );
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
    Input, int N, the number of vertices. MUST BE counter-clockwise
    Input, double X[N], Y[N], the coordinates of each vertex.

    Output, int TRIANGLES[3*(N-2)], the triangles of the triangulation.
*/
#define angle_tol 5.7E-05
MYIDEF triangles_t *polygon_triangulate(const polygon_t* cs)
{
    const vidx_t n = cs->n;
    // We must have at least 3 vertices.
    if (n < 3) {
        ERR("POLYGON_TRIANGULATE - Fatal error!  N < 3." );
        return NULL;
    }
    // Consecutive vertices cannot be equal.
    for (vidx_t curt = 0, prev = n-1; curt < n; curt++ ) {
        if ( coord_seq_getx(cs, prev) == coord_seq_getx(cs, curt) && coord_seq_gety(cs, prev) == coord_seq_gety(cs, curt) ) {
          ERR("POLYGON_TRIANGULATE - Fatal error!  Two consecutive nodes are identical." );
          return NULL;
        }
        prev = curt;
    }
    // No node can be the vertex of an angle less than 1 degree 
    // in absolute value.
    for (vidx_t curt = 0, prev = n-1; curt < n; curt++ ) {
        vidx_t next = ( ( curt + 1 ) % n ); 

        __auto_type angle = angle_degree(coord_seq_getx(cs, prev), coord_seq_gety(cs, prev), 
                                         coord_seq_getx(cs, curt), coord_seq_gety(cs, curt), 
                                         coord_seq_getx(cs, next), coord_seq_gety(cs, next) );

        if ( THE_ABS( angle ) <= angle_tol ) {
            ERR("POLYGON_TRIANGULATE - Fatal error! Polygon has an angle smaller than %g, accurring at node %d", angle_tol, curt);
            return NULL;
        }
        prev = curt;
    }
    // Area must be positive.
    if (signed_area(cs) <= 0.0) {
        ERR("POLYGON_TRIANGULATE - Fatal error!  Polygon has zero or negative area." );
        return NULL;
    }

    triangles_t* triangles = allocate_triangles(n - 2);

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
        ear[i] = diagonal(prev_node[i], next_node[i], prev_node, next_node, cs);
    }

    vidx_t triangle_idx = 0;

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
            ear[i1] = diagonal ( i0, i3, prev_node, next_node, cs);
            ear[i3] = diagonal ( i1, i4, prev_node, next_node, cs);
            // Add the diagonal [I3, I1, I2] to the list.
            triangles->vidx[0+triangle_idx*3] = i3;
            triangles->vidx[1+triangle_idx*3] = i1;
            triangles->vidx[2+triangle_idx*3] = i2;
            ++triangle_idx;
        }
        // Try the next vertex.
        i2 = next_node[i2];
    }
    // The last triangle is formed from the three remaining vertices.
    i3 = next_node[i2];
    i1 = prev_node[i2];

    triangles->vidx[0+triangle_idx*3] = i3;
    triangles->vidx[1+triangle_idx*3] = i1;
    triangles->vidx[2+triangle_idx*3] = i2;

    free ( ear );
    free ( next_node );
    free ( prev_node );

    triangles->m = triangle_idx + 1;
    return triangles;
}

#endif // POLY2TRI_IMPLEMENTATION
