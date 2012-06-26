#pragma once

#include "StabLib/Geom/inc/Point.h"
#include "StabLib/ImageProc/inc/Picture.h"
#include "fast_global_kernel.h"

class ObjectTracker
{
	track_target target;
	fgk_track_data* track_data;
	void initTarget(Point2D const& p1, Point2D const& p2);
public:
	ObjectTracker();
	~ObjectTracker();
	void init(Picture pic, Point2D const& p1, Point2D const& p2);
	void step(Picture pic);
	Point2D getP1() const;
	Point2D getP2() const;
	bool initialized() const;
	void dispose();
};