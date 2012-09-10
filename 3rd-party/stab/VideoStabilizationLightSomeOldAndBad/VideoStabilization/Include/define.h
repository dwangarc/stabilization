#pragma once

#include <boost/shared_ptr.hpp>


#define			GRAVITY_MODE		0
#define			LEFT_CHECK_MODE		1
#define			FORWARD_BACKWARD	2
#define			MANUAL				3

#define VPAGE_SOURCE		0
#define VPAGE_RECT_L		1
#define VPAGE_RECT_R		2
#define VPAGE_DEPTH			3
#define VPAGE_OCCUP_GRID	4

#define			ABS(x)										( x >= 0 ? x : -(x) )
#define			NULL	0

#define			UCHAR_TRUNC(z)								( (z > 0) && (z<256) ? z : 0 )

#define			TRUNC_L( x )								( x < 0 ? 0 : x)
#define			TRUNC_LW( x, width )						( x < width ? width : x)
#define			TRUNC_R( x, width )							( x >= width ? width - 1 : x)

#define			IF_TRUNC(d, dmin, dmax, x, y, trunc_r)		(	(d < dmin) || (d > dmax) || (x<trunc_r->xmin) || (x>trunc_r->xmax) || (y<trunc_r->ymin) || (y>trunc_r->ymax)	)

#define			MIN(x, y)									(x<y ? x : y)
#define			MAX(a, b)									(a > b ? a : b)


#define			Z(focus, disp, dmin)						(focus/ (float)( disp<dmin ? dmin : disp) )
#define			X(x, disp, d_width, o_width, dmin)			( (x - d_width/2)/(float)( disp<dmin ? dmin : disp) + o_width/2 )
#define			Y(y, disp, d_height, o_height, dmin)		( (y - d_height/2)/(float)( disp<dmin ? dmin : disp) + o_height/2 )


#define			ELEM( x, y, width )							( y * width + x)


#define			M_2PI										(2.0*M_PI)

typedef			unsigned char		uchar;

typedef struct obst_object {
	double		x1,y1,z1;				// left corner of obstacle
	double		x2,y2,z2;				// right corner of obstacle

	double		prjx[4], prjy[4], prjz[4];		// projection to ground plane

	double		dist;					// distance to object
} obst_object;

typedef struct ground_obst {
	double x[4];
	double z[4];
} ground_obst;

typedef struct trunc_rect {
	int			xmin,ymin;				// left corner of obstacle
	int			xmax,ymax;				// right corner of obstacle
} trunc_rect;

typedef boost::shared_ptr<unsigned char> sh_ptr_uch;
typedef boost::shared_ptr<double> sh_ptr_db;
typedef boost::shared_ptr<int> sh_ptr_int;
