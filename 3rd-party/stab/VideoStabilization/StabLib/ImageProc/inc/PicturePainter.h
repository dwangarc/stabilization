#pragma once

#include "Color.h"
#include "Picture.h"

class PicturePainter
{
	void setDotFast(Picture& pic, Point2D const& point, Color const& color) const;
public:
	void setDot(Picture& pic, Point2D const& point, Color const& color) const;
	void drawLine(Picture& pic, Point2D const& p1, Point2D const& p2, Color const& color) const;

	void drawRectangle(Picture& pic, Point2D const& corner1, Point2D const& corner2, Color const& color) const;
	void drawRectangle(Picture& pic, Point2D const& center, double width, double height, Color const& color) const;

	void drawRectangleCross(Picture& pic, Point2D const& corner1, Point2D const& corner2, Color const& color) const;

	void markNonNull(Picture& pic, Picture mask, Color addColor, unsigned char eps = 10) const;

};
