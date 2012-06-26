#include "stdafx.h"
#include "../inc/PictureAlgs.h"
#include <omp.h>

void PictureAlgs::smooth(Picture& pic, int dist) const
{
#pragma omp parallel
	{
#pragma omp for
		for (int i = dist; i < pic.getWidth() - dist + 1; ++i)
		{
			for (int j = dist; j < pic.getHeight() - dist + 1; ++j)
			{
				if (pic.at(i, j, 0) == 1)
				{
					for (int k = -dist; k < dist; ++k)
					{
						for (int m = -dist; m < dist; ++m)
						{
							if (pic.at(i + k, j + m, 0) == 0)
							{
								pic.at(i + k, j + m, 0) = 2;
							}
						}
					}
				}
			}
		}
	}
#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i < pic.getWidth(); ++i)
		{
			for (int j = 0; j < pic.getHeight(); ++j)
			{
				if (pic.at(i, j, 0) == 2)
				{
					pic.at(i, j, 0) = 1;
				}
			}
		}
	}
}


void PictureAlgs::zeroSame(Picture& pic, Picture base, int threshold) const
{
#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i < pic.getWidth(); ++i)
		{
			for (int j = 0; j < pic.getHeight(); ++j)
			{
				int sum = 0;
				for (int c = 0; c < pic.getColors(); ++c)
				{
					int v1 = pic.at(i, j, c);
					int v2 = base.at(i, j, c);
					sum += abs(v1 - v2);
				}
				sum /= pic.getColors();
				if (sum < threshold)
				{
					for (int c = 0; c < pic.getColors(); ++c)
					{
						pic.at(i, j, c) = 0;
					}
				}
			}
		}
	}
}

void PictureAlgs::toBitMaskStepSlight(Picture& pic, double minVal, int maxVal, int step) const
{
	sh_ptr_int sums = createSums(pic, step, step, 0);
	for (int i = 0; i < pic.getHeight(); ++i)
	{
		int place = i * pic.getWidth();
		for (int j = 0; j < pic.getWidth(); ++j)
		{
			int tmpVal = 0;
			for (int k = -step / 2; k < -step / 2 + step; ++k)
			{
				if (i + k < 0 || i + k >= pic.getHeight()) continue;
				for (int l = -step / 2; l < -step / 2 + step; ++l)
				{
					if (j + l < 0 || j + l >= pic.getWidth()) continue;
					int ps = place + l + k * pic.getWidth();
					if (sums.get()[ps] > tmpVal)					
					{
						tmpVal = sums.get()[ps];
					}
				}
			}
			unsigned char newVal = tmpVal >= (minVal * step * step * pic.getColors()) / 10.0 ? 1 : 0;
			for (int c = 0; c < 3; ++c)
			{
				pic.at(j, i, c) = newVal;
			}
			++place;
		}
	}
	

}

int PictureAlgs::sumInRect(Picture pic, int w, int h, int szW, int szH) const
{
	int sum = 0;
	for (int k = 0; k < szW; ++k)
	{
		for (int l = 0; l < szH; ++l)
		{
			for (int c = 0; c < pic.getColors(); ++c)
			{
				sum += pic.at(w + k, h + l, c);
			}
		}
	}
	return sum;
}



void PictureAlgs::toBitMaskStep(Picture& pic, int minVal, int maxVal, int step) const
{
#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i < pic.getWidth() - step; i += step)
		{
			for (int j = 0; j < pic.getHeight() - step; j += step)
			{
				int sum = sumInRect(pic, i, j, step, step);
//				sum /= pic.getColors();
				unsigned char newVal = sum  > minVal * step * step * pic.getColors() / 10.0 ? 1 : 0;
				pic.fill(newVal, i, j, i + step, j + step);
			}
		}
	}
}


void PictureAlgs::toBitMask(Picture& pic, int threshold) const
{
#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i < pic.getWidth(); ++i)
		{
			for (int j = 0; j < pic.getHeight(); ++j)
			{
				int sum = 0;
				for (int c = 0; c < pic.getColors(); ++c)
				{
					sum += pic.at(i, j, c);
				}
				sum /= pic.getColors();
				for (int c = 0; c < pic.getColors(); ++c)
				{
					pic.at(i, j, c) = 0;
				}
				pic.at(i, j, 0) = sum  > threshold ? 1 : 0;
			}
		}
	}
}

void PictureAlgs::fromBitMask(Picture& pic, Picture base) const
{
#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i < pic.getWidth(); ++i)
		{
			for (int j = 0; j < pic.getHeight(); ++j)
			{
				if (pic.at(i, j, 0) > 0)
				{
					for (int c = 0; c < pic.getColors(); ++c)
					{
						pic.at(i, j, c) = base.at(i, j, c);
					}
				}
			}
		}
	}
}

sh_ptr_int PictureAlgs::createSums(Picture pic, int szW, int szH, int bandWidth) const
{
	sh_ptr_int sum = createSubSums(pic, bandWidth);
	sh_ptr_int sums = sh_ptr_int(new int[pic.getWidth() * pic.getHeight()]);
	int shift1 = szW;
	int shift2 = szH * pic.getWidth();
	int shift3 = shift1 + shift2;
	for (int i = bandWidth; i < pic.getHeight() - bandWidth; ++i)
	{
		int place = i * pic.getWidth();
		for (int j = bandWidth; j < pic.getWidth() - bandWidth; ++j)
		{
			sums.get()[place] = sum.get()[place] - sum.get()[std::max(0, place - shift1)] - sum.get()[std::max(0, place - shift2)] + sum.get()[std::max(0, place - shift3)];
			++place;
		}
	}
	return sums;
}


sh_ptr_int PictureAlgs::createSubSums(Picture pic, int bandWidth) const
{
	sh_ptr_int sum = sh_ptr_int(new int[pic.getWidth() * pic.getHeight()]);
	memset(sum.get(), 0, pic.getWidth() * pic.getHeight() * sizeof(int));

	for (int i = bandWidth; i < pic.getHeight() - bandWidth; ++i)
	{
		int place = i * pic.getWidth();
		for (int j = bandWidth; j < pic.getWidth() - bandWidth; ++j)
		{
			sum.get()[place] = sum.get()[std::max(0, place - 1)] + sum.get()[std::max(0, place - pic.getWidth())] - sum.get()[std::max(0, place - pic.getWidth() - 1)];
			for (int c = 0; c < pic.getColors(); ++c)
			{
				sum.get()[place] += pic.get(j, i, c);
			}
			++place;
		}
	}
	return sum;
}


Point2Di PictureAlgs::findMax(Picture pic, Point2Di rectSize, int threshold, int bandWidth) const
{
	sh_ptr_int sum = createSums(pic, rectSize.x, rectSize.y, bandWidth);

	int maxSum = 0;
	Point2Di res;
	for (int i = bandWidth; i < pic.getHeight() - bandWidth; ++i)
	{
		int place = i * pic.getWidth();
		for (int j = bandWidth; j < pic.getWidth() - bandWidth; ++j)
		{
			if (sum.get()[place] > maxSum)
			{
				maxSum = sum.get()[place];
				res = Point2Di(j, i);
			}
			++place;
		}
	}
	if (maxSum > threshold * rectSize.x * rectSize.y * pic.getColors())
	{
		return res;	
	}
	return Point2Di();
}


lst_bfs PictureAlgs::bfsClean(Picture& pic, int minSize, int allowVal, int changeVal, int dfsStep)
{
	height = pic.getHeight();
	width = pic.getWidth();
	initGrid(height, width);
	lst_bfs islands = lst_bfs();
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			if (pic.get(j, i, 0) == allowVal && grid[i][j] == 0)
			{
				BfsOutcome res = bfs(Point2Di(i, j), pic, false, allowVal, changeVal, dfsStep);
				if (res.size < minSize)
				{
					bfs(Point2Di(i, j), pic, true, allowVal, changeVal, dfsStep);
				}
				else
				{
					islands.push_back(res);
				}
			}
		}
	}
	freeGrid(height);
	return islands;
}


lst_bfs PictureAlgs::excludeSmallIslands(Picture& pic, int minSize, int dfsStep )
{
	return bfsClean(pic, minSize, 0, 1, dfsStep);
}

lst_bfs PictureAlgs::excludeSmallSpaces(Picture& pic, int minSize, int dfsStep )
{
	return bfsClean(pic, minSize, 1, 0, dfsStep);
}



void PictureAlgs::updateOutCome(BfsOutcome& out, int x, int y) const
{
	if (x < out.pMin.x) out.pMin.x = x;
	if (y < out.pMin.y) out.pMin.y = y;
	if (x > out.pMax.x) out.pMax.x = x;
	if (y > out.pMax.y) out.pMax.y = y;
}


BfsOutcome PictureAlgs::bfs(Point2Di p, Picture pic, bool clear, int allowVal, int changeVal, int dfsStep )
{
	BfsOutcome res;
	res.pMax = p;
	res.pMin = p;
	res.size = 1;

	bottom = 0;
	top = 1;
	stack[bottom] = p;
	grid[p.x][p.y] = 1;
	if (clear)
	{
		pic.at(p.y, p.x, 0) = changeVal;
	}
	while (bottom < top)
	{
		int x = stack[bottom].x;
		int y = stack[bottom].y;
		for (int i = -dfsStep; i < dfsStep + 1; ++i)
		{
			for (int j = -dfsStep; j < dfsStep + 1; ++j)
			{
				if (x + i >= 0 && y + j >= 0 && x + i < height && y + j < width)
				{
					if ((grid[x + i][y + j] == (clear ? 1 : 0)) && (pic.get(y + j, x + i, 0) == allowVal))
					{
						res.size++;
						stack[top++] = Point2Di(x + i, y + j);
						grid[x + i][y + j] = 1;
						updateOutCome(res, x + i, y + j);
						if (clear)
						{
							pic.at(y + j, x + i, 0) = changeVal;
						}						
					}
				}
			}
		}
		++bottom;
	}
	return res;
}



void PictureAlgs::initGrid(int height, int width)
{
	stack = new Point2Di[height * width];
	grid = new int*[height];
	for (int i =0 ; i < height; ++i)
	{
		grid[i] = new int[width];
		for (int j = 0; j < width; ++j)
		{
			grid[i][j] = 0;
		}
	}
}

void PictureAlgs::freeGrid(int height)
{
	for (int j = 0; j < height; ++j)
	{
		delete[] grid[j];
	}
	delete[] grid;
	delete[] stack;
}
