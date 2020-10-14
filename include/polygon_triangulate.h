#pragma once

int *polygon_triangulate ( int n, double x[], double y[] );

double triangle_area ( double xa, double ya, double xb, double yb, double xc, double yc );
double angle_degree ( double x1, double y1, double x2, double y2, double x3, double y3 );
int between ( double xa, double ya, double xb, double yb, double xc, double yc );
int collinear ( double xa, double ya, double xb, double yb, double xc, double yc );
int diagonal ( int im1, int ip1, int n, int prev[], int next[], double x[], double y[] );
int diagonalie ( int im1, int ip1, int n, int next[], double x[], double y[] );
int in_cone ( int im1, int ip1, int n, int prev[], int next[], double x[], double y[] );
int intersect ( double xa, double ya, double xb, double yb, double xc, double yc, double xd, double yd );
int intersect_prop ( double xa, double ya, double xb, double yb, double xc, double yc, double xd, double yd );
double polygon_area ( int n, double x[], double y[] );

