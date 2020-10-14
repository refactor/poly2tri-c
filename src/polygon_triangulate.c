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
double angle_degree ( double x1, double y1, double x2, double y2, double x3, double y3 )
{
  double r8_pi = 3.141592653589793;
  double value;
  double x;
  double y;

  x = ( x3 - x2 ) * ( x1 - x2 ) + ( y3 - y2 ) * ( y1 - y2 );

  y = ( x3 - x2 ) * ( y1 - y2 ) - ( y3 - y2 ) * ( x1 - x2 );

  if ( x == 0.0 && y == 0.0 )
  {
    value = 0.0;
  }
  else
  {
    value = atan2 ( y, x );

    if ( value < 0.0 )
    {
      value = value + 2.0 * r8_pi;
    }
    value = 180.0 * value / r8_pi;
  }

  return value;
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
int between ( double xa, double ya, double xb, double yb, double xc, double yc )
{
  int value;
  double xmax;
  double xmin;
  double ymax;
  double ymin;

  if ( ! collinear ( xa, ya, xb, yb, xc, yc ) )
  {
    value = 0;
  }
  else if ( fabs ( ya - yb ) < fabs ( xa - xb ) )
  {
    xmax = THE_MAX ( xa, xb );
    xmin = THE_MIN ( xa, xb );
    value = ( xmin <= xc && xc <= xmax );
  }
  else
  {
    ymax = THE_MAX ( ya, yb );
    ymin = THE_MAX ( ya, yb );
    value = ( ymin <= yc && yc <= ymax );
  }

  return value;
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
int collinear ( double xa, double ya, double xb, double yb, double xc, double yc )
{
  double area;
  const double r8_eps = 2.220446049250313E-016;
  double side_ab_sq;
  double side_bc_sq;
  double side_ca_sq;
  double side_max_sq;
  int value;

  area = triangle_area ( xa, ya, xb, yb, xc, yc );

  side_ab_sq = pow ( xa - xb, 2 ) + pow ( ya - yb, 2 );
  side_bc_sq = pow ( xb - xc, 2 ) + pow ( yb - yc, 2 );
  side_ca_sq = pow ( xc - xa, 2 ) + pow ( yc - ya, 2 );

  side_max_sq = THE_MAX ( side_ab_sq, THE_MAX ( side_bc_sq, side_ca_sq ) );

  if ( side_max_sq <= r8_eps )
  {
    value = 1;
  }
  else if ( 2.0 * fabs ( area ) <= r8_eps * side_max_sq )
  {
    value = 1;
  }
  else
  {
    value = 0;
  }

  return value;
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
    Input, int N, the number of vertices.
    Input, int PREV_NODE[N], the previous neighbor of each vertex.
    Input, int NEXT_NODE[N], the next neighbor of each vertex.
    Input, double X[N], Y[N], the coordinates of each vertex.

    Output, int DIAGONAL, the value of the test.
*/
int diagonal ( int im1, int ip1, int n, int prev_node[], int next_node[], double x[], double y[] )
{
  int value;
  int value1;
  int value2;
  int value3;

  value1 = in_cone ( im1, ip1, n, prev_node, next_node, x, y );
  value2 = in_cone ( ip1, im1, n, prev_node, next_node, x, y );
  value3 = diagonalie ( im1, ip1, n, next_node, x, y );

  value = ( value1 && value2 && value3 );

  return value;
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
int diagonalie ( int im1, int ip1, int n, int next_node[], double x[], double y[] )
{
  int first;
  int j;
  int jp1;
  int value;
  int value2;

  first = im1;
  j = first;
  jp1 = next_node[first];

  value = 1;
/*
  For each edge VERTEX(J):VERTEX(JP1) of the polygon:
*/
  while ( 1 )
  {
/*
  Skip any edge that includes vertex IM1 or IP1.
*/
    if ( j == im1 || j == ip1 || jp1 == im1 || jp1 == ip1 )
    {
    }
    else
    {
      value2 = intersect ( x[im1], y[im1], x[ip1], y[ip1], x[j], y[j], 
        x[jp1], y[jp1] );

      if ( value2 )
      {
        value = 0;
        break;
      }
    }
    j = jp1;
    jp1 = next_node[j];

    if ( j == first )
    {
      break;
    }
  }

  return value;
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
    Input, int IM1, IP1, the indices of two vertices.
    Input, int N, the number of vertices.
    Input, int PREV_NODE[N], the previous neighbor of each vertex.
    Input, int NEXT_NODE[N], the next neighbor of each vertex.
    Input, double X[N], Y[N], the coordinates of each vertex.

    Output, int IN_CONE, the value of the test.
*/
int in_cone ( int im1, int ip1, int n, int prev_node[], int next_node[], double x[], double y[] )
{
  int i;
  int im2;
  double t1;
  double t2;
  double t3;
  double t4;
  double t5;
  int value;

  im2 = prev_node[im1];
  i = next_node[im1];

  t1 = triangle_area ( x[im1], y[im1], x[i], y[i], x[im2], y[im2] );

  if ( 0.0 <= t1 )
  {
    t2 = triangle_area ( x[im1], y[im1], x[ip1], y[ip1], x[im2], y[im2] );
    t3 = triangle_area ( x[ip1], y[ip1], x[im1], y[im1], x[i], y[i] );
    value = ( ( 0.0 < t2 ) && ( 0.0 < t3 ) );
  }
  else
  {
    t4 = triangle_area ( x[im1], y[im1], x[ip1], y[ip1], x[i], y[i] );
    t5 = triangle_area ( x[ip1], y[ip1], x[im1], y[im1], x[im2], y[im2] );
    value = ! ( ( 0.0 <= t4 ) && ( 0.0 <= t5 ) );
  }
  return value;
}

/**
  Purpose:
    INTERSECT is true if lines VA:VB and VC:VD intersect.

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
int intersect ( double xa, double ya, double xb, double yb, double xc, double yc, double xd, double yd )
{
  int value;

  if ( intersect_prop ( xa, ya, xb, yb, xc, yc, xd, yd ) )
  {
    value = 1;
  }
  else if ( between ( xa, ya, xb, yb, xc, yc ) )
  {
    value = 1;
  }
  else if ( between ( xa, ya, xb, yb, xd, yd ) )
  {
    value = 1;
  }
  else if ( between ( xc, yc, xd, yd, xa, ya ) )
  {
    value = 1;
  }
  else if ( between ( xc, yc, xd, yd, xb, yb ) )
  {
    value = 1;
  }
  else
  {
    value = 0;
  }
  return value;
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
int l4_xor ( int l1, int l2 )
{
  int value;
  int value1;
  int value2;

  value1 = (     l1   && ( ! l2 ) );
  value2 = ( ( ! l1 ) &&     l2   );

  value = ( value1 || value2 );

  return value;
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
int intersect_prop ( double xa, double ya, double xb, double yb, double xc, double yc, double xd, double yd )
{
  double t1;
  double t2;
  double t3;
  double t4;
  int value;
  int value1;
  int value2;
  int value3;
  int value4;

  if ( collinear ( xa, ya, xb, yb, xc, yc ) )
  {
    value = 0;
  }
  else if ( collinear ( xa, ya, xb, yb, xd, yd ) )
  {
    value = 0;
  }
  else if ( collinear ( xc, yc, xd, yd, xa, ya ) )
  {
    value = 0;
  }
  else if ( collinear ( xc, yc, xd, yd, xb, yb ) )
  {
    value = 0;
  }
  else
  {
    t1 = triangle_area ( xa, ya, xb, yb, xc, yc );
    t2 = triangle_area ( xa, ya, xb, yb, xd, yd );
    t3 = triangle_area ( xc, yc, xd, yd, xa, ya );
    t4 = triangle_area ( xc, yc, xd, yd, xb, yb );

    value1 = ( 0.0 < t1 );
    value2 = ( 0.0 < t2 );
    value3 = ( 0.0 < t3 );
    value4 = ( 0.0 < t4 );

    value = ( l4_xor ( value1, value2 ) ) && ( l4_xor ( value3, value4 ) );
  }
  return value;
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
double polygon_area ( int n, double x[], double y[] )
{
  double area;
  int i;
  int im1;

  area = 0.0;
  im1 = n - 1;
  for ( i = 0; i < n; i++ )
  {
    area = area + x[im1] * y[i] - x[i] * y[im1];
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
int *polygon_triangulate ( int n, double x[], double y[] )
{
  double angle;
  const double angle_tol = 5.7E-05;
  double area;
  int *ear;
  int i0;
  int i1;
  int i2;
  int i3;
  int i4;
  int *next_node;
  int node3;
  int *prev_node;
  int triangle_num;
  int *triangles;
/*
  We must have at least 3 vertices.
*/
  if ( n < 3 ) {
    ERR("POLYGON_TRIANGULATE - Fatal error!  N < 3." );
    return NULL;
  }
/*
  Consecutive vertices cannot be equal.
*/
  for (int node = 0, prev = n-1; node < n; node++ ) {
    if ( x[prev] == x[node] && y[prev] == y[node] ) {
      ERR("POLYGON_TRIANGULATE - Fatal error!  Two consecutive nodes are identical." );
      return NULL;
    }
    prev = node;
  }
/*
  No node can be the vertex of an angle less than 1 degree 
  in absolute value.
*/
  for (int node2 = 0, prev = n-1; node2 < n; node2++ ) {
    node3 = ( ( node2 + 1 ) % n ); 

    angle = angle_degree ( 
      x[prev], y[prev], 
      x[node2], y[node2], 
      x[node3], y[node3] );

    if ( fabs ( angle ) <= angle_tol )
    {
      ERR("POLYGON_TRIANGULATE - Fatal error!  Polygon has an angle smaller than %g,  accurring at node %d", angle_tol, node2);
      return NULL;
    }
    prev = node2;
  }
/*
  Area must be positive.
*/
  area = polygon_area ( n, x, y );
  if ( area <= 0.0 ) {
    ERR("POLYGON_TRIANGULATE - Fatal error!  Polygon has zero or negative area." );
    return NULL;
  }

  triangles = ( int * ) malloc ( 3 * ( n - 2 ) * sizeof ( int ) );
/*
  PREV_NODE and NEXT_NODE point to the previous and next nodes.
*/
  prev_node = ( int * ) malloc ( n * sizeof ( int ) );
  next_node = ( int * ) malloc ( n * sizeof ( int ) );

  int i = 0;
  prev_node[i] = n - 1;
  next_node[i] = i + 1;

  for ( i = 1; i < n - 1; i++ )
  {
    prev_node[i] = i - 1;
    next_node[i] = i + 1;
  }

  i = n - 1;
  prev_node[i] = i - 1;
  next_node[i] = 0;
/*
  EAR indicates whether the node and its immediate neighbors form an ear
  that can be sliced off immediately.
*/
  ear = ( int * ) malloc ( n * sizeof ( int ) );
  for ( i = 0; i < n; i++ )
  {
    ear[i] = diagonal ( prev_node[i], next_node[i], n, prev_node, next_node, x, y );
  }

  triangle_num = 0;

  i2 = 0;

  while ( triangle_num < n - 3 )
  {
/*
  If I2 is an ear, gather information necessary to carry out
  the slicing operation and subsequent "healing".
*/
    if ( ear[i2] )
    {
      i3 = next_node[i2];
      i4 = next_node[i3];
      i1 = prev_node[i2];
      i0 = prev_node[i1];
/*
  Make vertex I2 disappear.
*/
      next_node[i1] = i3;
      prev_node[i3] = i1;
/*
  Update the earity of I1 and I3, because I2 disappeared.
*/
      ear[i1] = diagonal ( i0, i3, n, prev_node, next_node, x, y );
      ear[i3] = diagonal ( i1, i4, n, prev_node, next_node, x, y );
/*
  Add the diagonal [I3, I1, I2] to the list.
*/
      triangles[0+triangle_num*3] = i3;
      triangles[1+triangle_num*3] = i1;
      triangles[2+triangle_num*3] = i2;
      triangle_num = triangle_num + 1;
    }
/*
  Try the next vertex.
*/
    i2 = next_node[i2];
  }
/*
  The last triangle is formed from the three remaining vertices.
*/
  i3 = next_node[i2];
  i1 = prev_node[i2];

  triangles[0+triangle_num*3] = i3;
  triangles[1+triangle_num*3] = i1;
  triangles[2+triangle_num*3] = i2;
  triangle_num = triangle_num + 1;

  free ( ear );
  free ( next_node );
  free ( prev_node );

  return triangles;
}
