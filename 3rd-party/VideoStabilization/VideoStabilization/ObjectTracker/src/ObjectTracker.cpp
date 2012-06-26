#include "stdafx.h"
#include "../inc/ObjectTracker.h"


void ObjectTracker::initTarget(Point2D const& p1, Point2D const& p2)
{
	target.h = (int)std::abs(p1.y - p2.y);
	target.w = (int)std::abs(p1.x - p2.x);
	target.x = (int)(p1.x + p2.x) / 2;
	target.y = (int)(p1.y + p2.y) / 2;
}

void ObjectTracker::dispose()
{
	fgk_track_release(track_data);
	track_data = 0;
}

ObjectTracker::ObjectTracker()
{
	track_data = 0;
}

ObjectTracker::~ObjectTracker()
{
	if (track_data)
	{
		dispose();
	}
}

void ObjectTracker::init(Picture pic, Point2D const& p1, Point2D const& p2)
{
	initTarget(p1, p2);
	if (track_data) 
	{
		dispose();
	}
	track_data = (fgk_track_data*)fgk_track_create(pic.getWidth(), pic.getHeight());
	fgk_track_init(track_data, pic.getPictureChar(), pic.getWidth() * pic.getColors(), &target);
}

void ObjectTracker::step(Picture pic)
{
	fgk_track_track(track_data, pic.getPictureChar(), pic.getWidth() * pic.getColors(), &target);
}

Point2D ObjectTracker::getP1() const
{
	return Point2D(target.x - target.w / 2, target.y - target.h / 2);
}

Point2D ObjectTracker::getP2() const
{
	return Point2D(target.x + target.w / 2, target.y + target.h / 2);
}

bool ObjectTracker::initialized() const
{
	if (track_data) return true;
	return false;
}
