#include "stdafx.h"
#include "../inc/PicturePainter.h"

void PicturePainter::setDot(Picture& pic, Point2D const& point, Color const& color) const
{
	if (point.outOfRange(0, 0, pic.getWidth() - 1, pic.getHeight() - 1)) return;
	for (int c = 0; c < pic.getColors(); ++c)
	{
		pic.at(point, c) = color.rgb[c];
	}
}

void PicturePainter::setDotFast(Picture& pic, Point2D const& point, Color const& color) const
{
	for (int c = 0; c < pic.getColors(); ++c)
	{
		pic.at(point, c) = color.rgb[c];
	}
}

void PicturePainter::drawLine(Picture& pic, Point2D const& p1, Point2D const& p2, Color const& color) const
{
	if (p1.outOfRange(0, 0, pic.getWidth() - 1, pic.getHeight() - 1)) return;
	if (p2.outOfRange(0, 0, pic.getWidth() - 1, pic.getHeight() - 1)) return;

	Point2D vec = p2 - p1;
	double leng = vec.length();
	int koeff = 2;
	vec *= 1 / (koeff * leng);
	Point2D curPoint = p1;
	for (int i = 0; i < koeff * leng; ++i)
	{
		setDotFast(pic, curPoint, color);
		curPoint += vec;
	}
}

void PicturePainter::drawRectangle(Picture& pic, Point2D const& p1, Point2D const& p2, Color const& color) const
{
	Point2D leftBottom(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
	Point2D rightTop(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
	Point2D leftTop(leftBottom.x, rightTop.y);
	Point2D rightBottom(rightTop.x, leftBottom.y);
	drawLine(pic, leftBottom, leftTop, color);
	drawLine(pic, leftBottom, rightBottom, color);
	drawLine(pic, rightTop, rightBottom, color);
	drawLine(pic, rightTop, leftTop, color);
}

void PicturePainter::drawRectangleCross(Picture& pic, Point2D const& p1, Point2D const& p2, Color const& color) const
{
	Point2D leftBottom(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
	Point2D rightTop(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
	Point2D leftTop(leftBottom.x, rightTop.y);
	Point2D rightBottom(rightTop.x, leftBottom.y);
	drawLine(pic, leftBottom, leftTop, color);
	drawLine(pic, leftBottom, rightBottom, color);
	drawLine(pic, rightTop, rightBottom, color);
	drawLine(pic, rightTop, leftTop, color);
	drawLine(pic, rightTop, leftBottom, color);
	drawLine(pic, rightBottom, leftTop, color);
}


void PicturePainter::drawRectangle(Picture& pic, Point2D const& center, double width, double height, Color const& color) const
{
	Point2D corner1(center.x + width / 2, center.y + height / 2);
	Point2D corner2(center.x - width / 2, center.y - height / 2);
	drawRectangle(pic, corner1, corner2, color);
}

void PicturePainter::markNonNull(Picture& pic, Picture mask, Color addColor, unsigned char eps) const
{
	if (mask.getWidth() != pic.getWidth() || mask.getHeight() != pic.getHeight() || mask.getColors() != pic.getColors())
	{
		return;
	}
#pragma omp parallel for
	for (int i = 0; i < pic.getHeight(); ++i)
	{
		for (int j = 0; j < pic.getWidth(); ++j)
		{
			int sum = 0;
			for (int c = 0; c < 3; ++c)
			{
				sum += mask.get(j, i, c);
			}
			if (sum > eps)
			{
				for (int c = 0; c < 3; ++c)
				{
					int newVal = pic.get(j, i, c);
					newVal += addColor.rgb[c];
					if (newVal > 255) newVal = 255;
					pic.at(j, i, c) = (unsigned char)newVal;
				}
			}
		}
	}

}

