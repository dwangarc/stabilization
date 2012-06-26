#pragma once

#include <list>

#include "Picture.h"
#include "Geom/inc/Point.h"

struct BfsOutcome
{
	int size;
	Point2Di pMin, pMax;
};

typedef std::list<BfsOutcome> lst_bfs;

class PictureAlgs
{
	int** grid;

	Point2Di* stack;
	int bottom, top;
	int width, height;

	void initGrid(int height, int width);
	void freeGrid(int height);

	BfsOutcome bfs(Point2Di p, Picture pic, bool clear, int allowVal, int changeVal, int dfsStep = 1);
	void updateOutCome(BfsOutcome& out, int x, int y) const;

	lst_bfs bfsClean(Picture& pic, int minSize, int allowVal, int changeVal, int dfsStep = 1);

	sh_ptr_int createSubSums(Picture pic, int bandWidth = 0) const;
	sh_ptr_int createSums(Picture pic, int szW, int szH, int bandWidth = 0) const;

public:
	void smooth(Picture& pic, int dist) const;
	void zeroSame(Picture& pic, Picture base, int threshold) const;
	void toBitMask(Picture& pic, int threshold) const;
	void toBitMaskStep(Picture& pic, int minVal, int maxVal, int step = 4) const;
	void toBitMaskStepSlight(Picture& pic, double minVal, int maxVal, int step = 4) const;
	void fromBitMask(Picture& pic, Picture base) const;

	int sumInRect(Picture pic, int w, int h, int szW, int szH) const;

	Point2Di findMax(Picture pic, Point2Di rectSize, int threshold = 0, int bandWidth = 16) const;


	lst_bfs excludeSmallIslands(Picture& pic, int minSize, int dfsStep = 1);
	lst_bfs excludeSmallSpaces(Picture& pic, int minSize, int dfsStep = 1);
};