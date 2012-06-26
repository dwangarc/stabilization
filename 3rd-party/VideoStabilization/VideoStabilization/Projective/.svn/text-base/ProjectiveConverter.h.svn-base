#pragma once

#include "ImageProc/inc/Picture.h"

class ProjectiveConverter
{

	bool createdMult;

	void createMemory();
	void freeMemory();

	static const int numThreads = 2;

public:
	ProjectiveConverter();
	~ProjectiveConverter();
	unsigned char* applyProj(unsigned char const* pic, int width, int height, int colors, double const* proj, unsigned char* data = 0) const;
	unsigned char* applyProjGrey(unsigned char const* pic, int width, int height, double const* proj, unsigned char* data = 0) const;
	unsigned char* applyProj(unsigned char const* pic, int width, int height, int colors, double const* proj, unsigned char* data, unsigned char const* doubleSizePic) const;
	unsigned char* applyProj2(unsigned char const* pic, int width, int height, int colors, double const* proj1, double const* proj2) const;
	Picture applyProj2(Picture pic, double const* proj1, double const* proj2) const;
	Picture applyProj(Picture pic, double const* proj);

	unsigned char* applyProjLight(unsigned char const* pic, int width, int height, int colors, double const* proj, unsigned char* data = 0) const;
	Picture applyProjLight(Picture pic, double const* proj) const;
	void applyProjLight(Picture pic, double const* proj, Picture res) const;

	void applyProj(Picture pic, double const* proj, Picture res);
	void resizeProj(double* proj, double koeff) const;

	Point2D applyProj(Point2D const& p1, double const* proj, int width, int height) const;
};