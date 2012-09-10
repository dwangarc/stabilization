#include "ProjectiveConverter.h"

#include "Include/capture_log.h"

#include <emmintrin.h>

#include <memory.h>
#include <omp.h>
#include <math.h>

//#define MAX_THREAD_NUM 2

unsigned char* ProjectiveConverter::applyProjLight(unsigned char const* pic, int width, int height, int colors, double const* proj, unsigned char* data) const
{
	unsigned char* res = data ? data : new unsigned char[width * height * colors];
	int halfWidth = width / 2;
	int halfHeight = height / 2;
	int widthM1 = width;
	int heightM1 = height;
	widthM1 -= 2;
	heightM1 -= 2;

	int shift = 4;
	int shift2 = shift * 2;
	int koeff = 1 << shift;
	int koeff2 = koeff << shift;
	int widthColors = width * colors;

#pragma omp parallel
	{
#pragma omp for
		for(int j = 0; j < height; ++j)
		{
			//			int place = omp_get_thread_num();

			double jp = j;
			jp -= halfHeight;
			double projJ1 = proj[1] * jp;
			double projJ4 = proj[4] * jp;
			double projJ7 = proj[7] * jp;
			projJ1 += proj[2];
			projJ4 += proj[5];
			projJ7 += 1;

			int jWidth = j * width;

			double ipP6 = -halfWidth * proj[6];
			double ipP3 = -halfWidth * proj[3];
			double ipP0 = -halfWidth * proj[0];

			ipP0 += projJ1;
			ipP3 += projJ4;
			ipP6 += projJ7;

			double x, y;
			int ix, iy, iyWidth;
			int ijWidthColors = jWidth * colors;

			double xStart = ipP0 / ipP6;
			double yStart = ipP3 / ipP6;
			double xEnd = (ipP0 + width * proj[0]) / (ipP6 + width * proj[6]);
			double yEnd = (ipP3 + width * proj[3]) / (ipP6 + width * proj[6]);
			double xAdd = (xEnd - xStart) / width;
			double yAdd = (yEnd - yStart) / width;
			x = ipP0 + halfWidth;
			y = ipP3 + halfHeight;

			for(int i = 0; i < width; ++i)
			{

				if (x < 0 || x > widthM1 || y < 0 || y > heightM1) 
				{
					for (int k = 0; k < colors; ++k)
					{
						res[ijWidthColors + k] = 0;
					}
					ijWidthColors += colors;
					continue;
				}

				ix = int(x);
				iy = int(y);
				iyWidth = iy;
				iyWidth *= width;

				ix <<= shift;
				iy <<= shift;
				int ix4((int)(x * koeff));
				int iy4((int)(y * koeff));
				int idx = ix4 - ix;
				int idy = iy4 - iy;
				int idxdy = idx * idy;
				idx <<= shift;
				idy <<= shift;

				int ixiyWidth = ((int)x) + iyWidth;
				int ixiyWidthColors = ixiyWidth * colors;
				int ixiyWidthColorsWC = ixiyWidthColors + widthColors;
				for (int k = 0; k < colors; ++k)
				{
					int res1 = (koeff2 - idx - idy + idxdy) * pic[ixiyWidthColors + k];
					res1 += (idx - idxdy) * pic[ixiyWidthColors + colors + k];
					res1 += (idy - idxdy) * pic[ixiyWidthColorsWC + k];
					res1 += idxdy * pic[ixiyWidthColorsWC + colors + k];
					res1 >>= shift2;				

					res[ijWidthColors + k] = res1;//pic[ixiyWidthColors + k];//res1;
				}

				ijWidthColors += colors;
				x += xAdd;
				y += yAdd;
			}
		}
	}


	return res;

}

void ProjectiveConverter::resizeProj(double* proj, double koeff) const
{
	proj[6] /= koeff;
	proj[7] /= koeff;
	proj[2] *= koeff;
	proj[5] *= koeff;
}


unsigned char* ProjectiveConverter::applyProj2(unsigned char const* pic, int width, int height, int colors, double const* proec, double const* newProec) const
{
	unsigned char* new_result_image = new unsigned char[3 * width * height];
	memset(new_result_image, 0, 3 * width * height * sizeof(unsigned char));
#pragma omp parallel
	{
#pragma omp for
		for(int j = 0; j < height; j++)
		{
			for(int i = 0; i < width; i++)
			{
				double jp = j - height * 0.5;
				double ip = i - width * 0.5;
				double x1 = (proec[0] * ip + proec[1] * jp + proec[2]) / 
					(proec[6] * ip + proec[7] * jp + proec[8]);
				double y1 = (proec[3] * ip + proec[4] * jp + proec[5]) / 
					(proec[6] * ip + proec[7] * jp + proec[8]);
				double x2 = (newProec[0] * ip + newProec[1] * jp + newProec[2]) / 
					(newProec[6] * ip + newProec[7] * jp + newProec[8]);
				double y2 = (newProec[3] * ip + newProec[4] * jp + newProec[5]) / 
					(newProec[6] * ip + newProec[7] * jp + newProec[8]);
				double x = x1 - x2 + i;
				double y = y1 - y2 + j;
				/*				y += height * 0.5;
				x += width * 0.5;*/
				if(x < 0 || x > width - 1 || y < 0 || y > height - 1)
					continue;
				int ix = int(x);
				int iy = int(y);
				double dx = x - ix;
				double dy = y - iy; 

				for (int k = 0; k < colors; ++k)
				{
//					new_result_image[(i + j * width) * colors + k] = pic[(ix + iy * width) * colors + k]; 
					new_result_image[(i + j * width) * colors + k] = unsigned char(
						(1 - dx) * (1 - dy) * pic[(ix + iy * width) * colors + k] + 
						dx * (1 - dy) * pic[(ix + 1 + iy * width) * colors + k] + 
						(1 - dx) * dy * pic[(ix + (iy + 1) * width) * colors + k] + 
						dx * dy * pic[(ix + 1 + (iy + 1) * width) * colors + k]); 
				}
			}
		}
	}
	return new_result_image;
}

Picture ProjectiveConverter::applyProj2(Picture pic, double const* proj1, double const* proj2) const
{
	return Picture(applyProj2(pic.getPictureChar(), pic.getWidth(), pic.getHeight(), pic.getColors(), proj1, proj2), pic.getWidth(), pic.getHeight(), pic.getColors());
}

Picture ProjectiveConverter::applyProj(Picture pic, double const* proj)
{
	return Picture(applyProj(pic.getPictureChar(), pic.getWidth(), pic.getHeight(), pic.getColors(), proj), pic.getWidth(), pic.getHeight(), pic.getColors());
}

Picture ProjectiveConverter::applyProjLight(Picture pic, double const* proj) const
{
	return Picture(applyProjLight(pic.getPictureChar(), pic.getWidth(), pic.getHeight(), pic.getColors(), proj), pic.getWidth(), pic.getHeight(), pic.getColors());
}

void ProjectiveConverter::applyProj(Picture pic, double const* proj, Picture res)
{
	applyProj(pic.getPictureChar(), pic.getWidth(), pic.getHeight(), pic.getColors(), proj, res.getPicture().get());
}

void ProjectiveConverter::applyProjLight(Picture pic, double const* proj, Picture res) const
{
	applyProjLight(pic.getPictureChar(), pic.getWidth(), pic.getHeight(), pic.getColors(), proj, res.getPicture().get());
}

Point2D ProjectiveConverter::applyProj(Point2D const& p1, double const* proec, int width, int height) const
{
	double jp = p1.y - height * 0.5;
	double ip = p1.x - width * 0.5;
	Point2D res;
	res.x = (proec[0] * ip + proec[1] * jp + proec[2]) / 
		(proec[6] * ip + proec[7] * jp + proec[8]);
	res.y = (proec[3] * ip + proec[4] * jp + proec[5]) / 
		(proec[6] * ip + proec[7] * jp + proec[8]);
	res.x += width * 0.5;
	res.y += height * 0.5;
	return res;
}

unsigned char* ProjectiveConverter::applyProjGrey(unsigned char const* pic, int width, int height, double const* proj, unsigned char* data) const
{
	unsigned char* res = data;
	if (!res)
	{
		res = new unsigned char[width * height];
		memset(res, 0, width * height * sizeof(unsigned char));
	}
	int halfWidth = width / 2;
	int halfHeight = height / 2;
	int halfHeightD16 = halfHeight * 16;
	int halfWidthD16 = halfWidth * 16;

	int widthM1 = width;
	int heightM1 = height;
	widthM1 -= 2;
	heightM1 -= 2;

	widthM1 *= 16;
	heightM1 *= 16;

	int shift = 4;
	int shift2 = shift * 2;
	int koeff = 1 << shift;
	int koeff2 = koeff << shift;

	int* multWidth = new int[height];
	multWidth[0] = 0;
	for (int i = 1; i < height; ++i)
	{
		multWidth[i] = multWidth[i - 1] + width;
	}

#pragma omp parallel
	{
#pragma omp for schedule(dynamic)
		for(int j = 0; j < height; ++j)
		{
			double jp = j;
			jp -= halfHeight;
			double projJ1 = proj[1] * jp;
			double projJ4 = proj[4] * jp;
			double projJ7 = proj[7] * jp;
			projJ1 += proj[2];
			projJ4 += proj[5];
			projJ7 += 1;

			int jWidth = j * width;

			double ipP6 = -halfWidth * proj[6];
			double ipP3 = -halfWidth * proj[3];
			double ipP0 = -halfWidth * proj[0];

			ipP0 += projJ1;
			ipP3 += projJ4;
			ipP6 += projJ7;

			int ijWidthColors = jWidth;

			for(int i = 0; i < width; ++i, ipP0 += proj[0], ipP3 += proj[3], ipP6 += proj[6], ++ijWidthColors)
			{
				double norm = 16.0 / ipP6;
				int ix16 = (int)(ipP0 * norm);
				int iy16 = (int)(ipP3 * norm);

				ix16 += halfWidthD16;
				iy16 += halfHeightD16;

				if (ix16 < 0 || ix16 > widthM1 || iy16 < 0 || iy16 > heightM1) continue;

				int ix = ix16 >> 4;
				int iy = iy16 >> 4;

				int iyWidth = multWidth[iy];
				int ixiyWidth = iyWidth + ix;
				int ixiyWidthColors = ixiyWidth;
				int ixiyWidthColorsWC = ixiyWidthColors + width;

				ix <<= 4;
				iy <<= 4;
				int idx = ix16 - ix;
				int idy = iy16 - iy;
				int idxdy = idx * idy;
				idx <<= 4;
				idy <<= 4;

				int res1 = (koeff2 - idx - idy + idxdy) * pic[ixiyWidthColors];
				res1 += (idx - idxdy) * pic[ixiyWidthColors + 1];
				res1 += (idy - idxdy) * pic[ixiyWidthColorsWC];
				res1 += idxdy * pic[ixiyWidthColorsWC + 1];
				res1 >>= shift2;				
				res[ijWidthColors] = res1;
			}
		}
	}

	delete[] multWidth;

	return res;

}


unsigned char* ProjectiveConverter::applyProj(unsigned char const* pic, int width, int height, int colors, double const* proj, unsigned char* data) const
{
	unsigned char* res = data;
	if (!res)
	{
		res = new unsigned char[width * height * colors];
		memset(res, 0, width * height * colors * sizeof(unsigned char));
	}
	int halfWidth = width / 2;
	int halfHeight = height / 2;
	int halfHeightD16 = halfHeight * 16;
	int halfWidthD16 = halfWidth * 16;

	int widthM1 = width;
	int heightM1 = height;
	widthM1 -= 2;
	heightM1 -= 2;

	widthM1 *= 16;
	heightM1 *= 16;

	int shift = 4;
	int shift2 = shift * 2;
	int koeff = 1 << shift;
	int koeff2 = koeff << shift;
	int widthColors = width * colors;

	int* multWidth = new int[height];
	multWidth[0] = 0;
	for (int i = 1; i < height; ++i)
	{
		multWidth[i] = multWidth[i - 1] + width;
	}

#pragma omp parallel
	{
#pragma omp for schedule(dynamic)
		for(int j = 0; j < height; ++j)
		{
			double jp = j;
			jp -= halfHeight;
			double projJ1 = proj[1] * jp;
			double projJ4 = proj[4] * jp;
			double projJ7 = proj[7] * jp;
			projJ1 += proj[2];
			projJ4 += proj[5];
			projJ7 += 1;

			int jWidth = j * width;

			double ipP6 = -halfWidth * proj[6];
			double ipP3 = -halfWidth * proj[3];
			double ipP0 = -halfWidth * proj[0];

			ipP0 += projJ1;
			ipP3 += projJ4;
			ipP6 += projJ7;

			int ijWidthColors = jWidth * colors;

			for(int i = 0; i < width; ++i, ipP0 += proj[0], ipP3 += proj[3], ipP6 += proj[6], ijWidthColors += colors)
			{
				double norm = 16.0 / ipP6;
				int ix16 = (int)(ipP0 * norm);
				int iy16 = (int)(ipP3 * norm);

				ix16 += halfWidthD16;
				iy16 += halfHeightD16;

				if (ix16 < 0 || ix16 > widthM1 || iy16 < 0 || iy16 > heightM1) continue;

				int ix = ix16 >> 4;
				int iy = iy16 >> 4;

				int iyWidth = multWidth[iy];
				int ixiyWidth = iyWidth + ix;
				int ixiyWidthColors = ixiyWidth * colors;
				int ixiyWidthColorsWC = ixiyWidthColors + widthColors;


				ix <<= 4;
				iy <<= 4;
				int idx = ix16 - ix;
				int idy = iy16 - iy;
				int idxdy = idx * idy;
				idx <<= 4;
				idy <<= 4;


				int kf1 = koeff2 - idx - idy + idxdy;
				for (int k = 0; k < colors; ++k)
				{
					int res1 = kf1 * pic[ixiyWidthColors + k];
					res1 += (idx - idxdy) * pic[ixiyWidthColors + colors + k];
					res1 += (idy - idxdy) * pic[ixiyWidthColorsWC + k];
					res1 += idxdy * pic[ixiyWidthColorsWC + colors + k];
					res1 >>= shift2;				
					res[ijWidthColors + k] = res1;
				}

			}

		}
	}

	delete[] multWidth;

	return res;
}

void ProjectiveConverter::createMemory()
{

}

void ProjectiveConverter::freeMemory()
{
}

ProjectiveConverter::ProjectiveConverter()
{
	createdMult = false;
}


ProjectiveConverter::~ProjectiveConverter()
{
	freeMemory();
}


unsigned char* ProjectiveConverter::applyProj(unsigned char const* pic, int width, int height, int colors, double const* proj, unsigned char* data, unsigned char const* doubleSizePic) const
{
	unsigned char* res = data;
	if (!res)
	{
		res = new unsigned char[width * height * colors];
		memset(res, 0, width * height * colors * sizeof(unsigned char));
	}
	int halfWidth = width / 2;
	int halfHeight = height / 2;
	int doubleWidth = width * 2;
	int widthM1 = width;
	int heightM1 = height;
	widthM1 -= 2;
	heightM1 -= 2;

	int shift = 4;
	int shift2 = shift * 2;
	int koeff = 1 << shift;
	int koeff2 = koeff << shift;
	int widthColors = width * colors;

#pragma omp parallel
	{
#pragma omp for
		for(int j = 0; j < height; ++j)
		{
			//			int place = omp_get_thread_num();

			double jp = j;
			jp -= halfHeight;
			double projJ1 = proj[1] * jp;
			double projJ4 = proj[4] * jp;
			double projJ7 = proj[7] * jp;
			projJ1 += proj[2];
			projJ4 += proj[5];
			projJ7 += 1;

			int jWidth = j * width;

			double ipP6 = -halfWidth * proj[6];
			double ipP3 = -halfWidth * proj[3];
			double ipP0 = -halfWidth * proj[0];

			ipP0 += projJ1;
			ipP3 += projJ4;
			ipP6 += projJ7;

			double x, y, norm;
			int ix, iy, iyWidth;
			int ijWidthColors = jWidth * colors;

			for(int i = 0; i < width; ++i)
			{
				norm = 1 / ipP6;

				x = ipP0;
				y = ipP3;

				x *= norm;
				y *= norm;

				x += halfWidth;
				y += halfHeight;

				if (x < 0 || x > widthM1 || y < 0 || y > heightM1) 
				{
					//for (int k = 0; k < colors; ++k)
					//{
					//	res[ijWidthColors + k] = 0;
					//}
					ipP0 += proj[0];
					ipP3 += proj[3];
					ipP6 += proj[6];
					ijWidthColors += colors;
					continue;
				}

				ix = int(x * 2);
				iy = int(y * 2);
				iyWidth = iy;
				iyWidth *= doubleWidth;

				int ixiyWidthColors = (iyWidth + ix) * colors;
				
				for (int k = 0; k < colors; ++k)
				{
					res[ijWidthColors + k] = doubleSizePic[ixiyWidthColors + k];
				}

				ipP0 += proj[0];
				ipP3 += proj[3];
				ipP6 += proj[6];
				ijWidthColors += colors;
			}
		}
	}


	return res;
}
