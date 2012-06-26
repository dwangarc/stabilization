#include "stdafx.h"
#include "../inc/pictureConverter.h"
//#include "image.h"

PictureConverter::PictureConverter()
{
	init = false;
	int maxPossibleSize = 2500;
	multWC = new int[maxPossibleSize];
	multC = new int[maxPossibleSize];
}

PictureConverter::~PictureConverter()
{
	delete[] multC;
	delete[] multWC;
}


void PictureConverter::initDivide()
{
	int place = 0;
	divide4 = sh_ptr_int(new int[256 * 4]);
	for (int i = 0; i < 256; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			divide4.get()[place++] = i;
		}
	}
	init = true;
}




Picture PictureConverter::convertGrey(Picture pic, int dim) const
{
	if (dim == 1 || pic.getColors() != 1)
	{
		return pic.clone();
	}

	int height = pic.getHeight();
	int width = pic.getWidth();
	Picture res(width, height, dim);

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			int brightness = 0;
			for (int k = 0; k < dim; ++k)
			{
				res.at(j, i, k) = pic.get(j, i);
			}
		}
	}
	return res;

}

Picture PictureConverter::toBGR(Picture pic) const
{
	Picture res(pic.getWidth(), pic.getHeight(), pic.getColors());
	for (int i = 0; i < pic.getWidth(); ++i)
	{
		for (int j = 0; j < pic.getHeight(); ++j)
		{
			for (int c = 0; c < pic.getColors(); ++c)
			{
				res.at(i, j, pic.getColors() - c - 1) = pic.get(i, j, c);
			}
		}
	}
	return res;
}


sh_prt_uch PictureConverter::createBitMapData(Picture pic, int alignIm) const
{
	int width = pic.getWidth();
	int height = pic.getHeight();
	int colors = pic.getColors();
	int step = width;
	step *= 3;
	step += step % alignIm == 0 ? 0 : alignIm - step % alignIm;
	sh_prt_uch btm = sh_prt_uch(new unsigned char[step * height]);
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			for (int c = 0; c < 3; ++c)
			{
				btm.get()[i * step + 3 * j + c] = pic.getPictureChar()[i * width * colors + j * colors + (colors != 1 ? c : 0)];
			}
		}
	}

	return btm;
}


//bitmap* PictureConverter::createBitMap(Picture pic) const
//{
//	int width = pic.getWidth();
//	int height = pic.getHeight();
//	int colors = pic.getColors();
//	bitmap* bitMapH = bitmap_create(width, height, 3);
//	for (int i = 0; i < height; ++i)
//	{
//		for (int j = 0; j < width; ++j)
//		{
//			for (int c = 0; c < 3; ++c)
//			{
//				bitMapH->data[i * bitMapH->step + 3 * j + c] = pic.getPictureChar()[i * width * colors + j * colors + (colors != 1 ? c : 0)];
//			}
//		}
//	}
//
//	return bitMapH;
//}


Picture PictureConverter::convertColor(Picture pic) const
{
	int width = pic.getWidth();
	int height = pic.getHeight();
	int dim = pic.getColors();
	Picture res(width, height);
#pragma omp parallel for
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			int brightness = 0;
			for (int k = 0; k < dim; ++k)
			{
				brightness += pic.get(j, i, k);
			}
			brightness /= dim;
			res.at(j, i) = (unsigned char)brightness;
		}
	}
	return res;
}


Picture PictureConverter::applyHomo(Picture pic, Homography  homo) const
{
	int width = pic.getWidth();
	int height = pic.getHeight();
	int colors = pic.getColors();
	Picture res(width, height, colors);
#pragma omp parallel for
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			Point2D xy(j - width / 2, i - height / 2);
			Point2D xy1 = homo.applyHomo(xy);
			xy1.x += width / 2;
			xy1.y += height / 2;
			if (xy1.x >= 0 && xy1.y >= 0 && xy1.x < width && xy1.y < height)
			{
				for (int c = 0; c < colors; ++c)
				{
					res.at(j, i, c) = (unsigned char)pic.getColor(xy1.x, xy1.y, c);
				}
			}
		}
	}

	return res;

}

Picture PictureConverter::downscale(Picture pic, int downscale)
{
	if (downscale == 1)
	{
		return pic.clone();
	}
	if (downscale == 2)
	{
		return downscale2Fast(pic);
	}
	return downscaleSimple(pic, downscale);
}

Picture PictureConverter::downscale2Fast(Picture pic1)
{
	if (!init) initDivide();

	int colors = pic1.getColors();
	int downsc2 = 2;
	int height = pic1.getHeight();
	int width = pic1.getWidth();
	int widthD = width / downsc2;
	int heightD = height / downsc2;
	int widthColors = width * colors;
	int colors2 = colors * 2;
	Picture res1(widthD, heightD, colors);
	unsigned char const* pic = pic1.getPictureChar();
	unsigned char* res = res1.getPicture().get();
#pragma omp parallel for
	for (int i = 0; i < heightD; ++i)
	{
		int iDownscale = i * downsc2;
		int jDownscale = 0;
		int place = i * colors * widthD;
		int picPlace = iDownscale * colors * width;
		for (int j = 0; j < widthD; ++j)
		{
			for (int c = 0; c < colors; ++c)
			{
				int sum = pic[picPlace + c];
				sum += pic[picPlace + colors + c];
				sum += pic[picPlace + widthColors + c];
				sum += pic[picPlace + widthColors + colors + c];
				sum = divide4.get()[sum];
				unsigned char val = (unsigned char)sum;
				res[place++] = val;
			}

			picPlace += colors2;
			jDownscale += downsc2;
		}
	}
	return res1;

}


Picture PictureConverter::downscale2(Picture  pic)
{
	if (!init) initDivide();

	int colors = pic.getColors();
	int downsc2 = 2;
	int widthD = pic.getWidth() / downsc2;
	int heightD = pic.getHeight() / downsc2;
	Picture res(widthD, heightD, colors);
#pragma omp parallel for
	for (int i = 0; i < heightD; ++i)
	{
		int iDownscale = i * downsc2;
		int jDownscale = 0;
		for (int j = 0; j < widthD; ++j)
		{

			for (int c = 0; c < colors; ++c)
			{
				int sum = 0;
				for (int g = 0; g < downsc2; ++g)
				{
					for (int k = 0; k < downsc2; ++k)
					{
						sum += pic.get(jDownscale + k, iDownscale + g, c);
					}
				}
				sum = divide4.get()[sum];
				unsigned char val = (unsigned char)sum;
				res.at(j, i, c) = val;
			}

			jDownscale += downsc2;
		}
	}
	return res;
}


Picture PictureConverter::downscaleSimple(Picture pic, int downscale)
{
	int colors = pic.getColors();
	int widthD = pic.getWidth() / downscale;
	int heightD = pic.getHeight() / downscale;
	Picture res(widthD, heightD, colors);
	int down2 = downscale * downscale;
#pragma omp parallel for
	for (int i = 0; i < heightD; ++i)
	{
		int iDownscale = i * downscale;
		int jDownscale = 0;
		for (int j = 0; j < widthD; ++j)
		{

			for (int c = 0; c < colors; ++c)
			{
				int sum = 0;
				for (int g = 0; g < downscale; ++g)
				{
					for (int k = 0; k < downscale; ++k)
					{
						sum += pic.get(jDownscale + k, iDownscale + g, c);
					}
				}
				sum /= down2;
				unsigned char val = (unsigned char)sum;
				res.at(j, i, c) = val;
			}
			
			jDownscale += downscale;
		}
	}
	return res;
}


Picture PictureConverter::unFixDistortions(StereoConverter const& converter, Picture pic)
{
	Picture res(pic.getWidth(), pic.getHeight(), pic.getColors());
#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i < pic.getWidth(); ++i)
		{
			for (int j = 0; j < pic.getHeight(); ++j)
			{
				Point3D p(i, j, 1);
				Point3D shift(converter.convert(p));
				if (shift.x >= 0 && shift.y >= 0 && shift.x < pic.getWidth() - 1 && shift.y < pic.getHeight() - 1)
				{
					for (int c = 0; c < pic.getColors(); ++c)
					{
						res.at((int)shift.x, (int)shift.y, c) = (unsigned char) pic.getColor(i, j, c);
					}
				}
			}
		}
	}

	return res;
}


Picture PictureConverter::fixDistortions(StereoConverter const& converter, Picture pic)
{
	Picture res(pic.getWidth(), pic.getHeight(), pic.getColors());
	int width = pic.getWidth();
	int height = pic.getHeight();
	int colors = pic.getColors();
	int widthColors = width * colors;

	unsigned char* res1 = res.getPicture().get();
	unsigned char const* pic1 = pic.getPictureChar();

	multWC[0] = 0;
	multC[0] = 0;
	for (int i = 1; i < height; ++i)
	{
		multWC[i] = multWC[i - 1] + widthColors;
	}
	for (int i = 1; i < width; ++i)
	{
		multC[i] = multC[i - 1] + colors;
	}


#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i < width; ++i)
		{
			int iH = i * height;
			int iHC = i * colors;
			for (int j = 0; j < height; ++j)
			{
//				Point3D p(i, j, 1);
//				Point3D shift(converter.convertFast(iH));
				Point2Di shift(converter.convertFastInt(iH));
				if (shift.x >= 0 && shift.y >= 0 && shift.x < width && shift.y < height)
				{
					for (int c = 0; c < colors; ++c)
					{
//						res.at(i, j, c) = pic.get((int)shift.x, (int)shift.y, c);

						res1[iHC + c] = pic1[multC[shift.x] + multWC[shift.y] + c];
//						res1[iHC + c] = pic.get(shift.x, shift.y, c);
//						res.at(i, j, c) = pic.get(shift.x, shift.y, c);
//						res.at(i, j, c) = (unsigned char)pic.getColor(/*(int)*/shift.x, /*(int)*/shift.y, c);
					}
				}
				iHC += widthColors;

				++iH;
			}
		}
	}
	return res;
}

Picture PictureConverter::normallize(Picture pic, int mediumInt) const
{
	int colors = pic.getColors();
	int width = pic.getWidth();
	int height = pic.getHeight();
	long long totalIntensity = 0;
#pragma omp parallel
	{
#pragma omp for reduction(+: totalIntensity)
		for (int i = 0; i < width; ++i)
		{
			for (int j = 0; j < height; ++j)
			{
				for (int c = 0; c < colors; ++c)
				{
					totalIntensity += pic.get(i, j, c);
				}
			}
		}
	}
	totalIntensity /= width;
	totalIntensity /= height;
	totalIntensity /= colors;
	double koeff = mediumInt;
	koeff /= totalIntensity;
	Picture res(width, height, colors);

#pragma omp parallel
	{
#pragma omp for 
		for (int i = 0; i < width; ++i)
		{
			for (int j = 0; j < height; ++j)
			{
				for (int c = 0; c < colors; ++c)
				{
					double val = pic.get(i, j, c);
					val *= koeff;
					if (val > 255) val = 255;
					if (val < 0) val = 0;
					res.at(i, j, c) = (unsigned char)val;
				}
			}
		}
	}

	return res;
}


Picture PictureConverter::stretchNearest2(Picture  pic) const
{
	int colors = pic.getColors();
	int widthD = pic.getWidth() * 2;
	int heightD = pic.getHeight() * 2;
	int width = pic.getWidth();
	int height = pic.getHeight();
	Picture res(widthD, heightD, colors);

	int widthDCol = widthD * colors;
	int widthCol = width * colors;
	int step = colors;
	unsigned char* resCh = res.getPicture().get();
	unsigned char const* baseCh = pic.getPictureChar();

#pragma omp parallel for schedule(static)
	for (int i = 0; i < height; ++i)
	{
		int plRes = i * width * colors * 4;
		int plBase = i * width * colors;

		for (int j = 0; j < width; ++j)
		{
			for (int c = 0; c < colors; ++c)
			{
				resCh[plRes] = baseCh[plBase];
				resCh[plRes + colors] = baseCh[plBase];
				resCh[plRes + widthDCol] = baseCh[plBase];
				resCh[plRes + widthDCol + colors] = baseCh[plBase];
				++plBase;
				++plRes;
			}
			plRes += colors;
		}
	}
	return res;

}


Picture PictureConverter::stretch(Picture  pic, int stretch) const
{
	int colors = pic.getColors();
	int widthD = pic.getWidth() * stretch;
	int heightD = pic.getHeight() * stretch;
	Picture res(widthD, heightD, colors);

	double stretchKoeff = 1.0 / stretch;
#pragma omp parallel for
	for (int i = 0; i < heightD; ++i)
	{
		double iStretch = ((double)i) / stretch;
		double jStretch = 0;
		for (int j = 0; j < widthD; ++j)
		{
			for (int c = 0; c < colors; ++c)
			{
				unsigned char vl1 = (unsigned char)pic.getColor(jStretch, iStretch, c);
				res.at(j, i, c) = vl1;
			}
			jStretch += stretchKoeff;
		}
	}
	return res;
}


Picture PictureConverter::fromSmallPic(Picture smallPic, Picture realPic, int downscale) const
{
	Picture res(realPic.getWidth(), realPic.getHeight(), realPic.getColors());
	res.fill(0);

#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i < smallPic.getWidth(); ++i)
		{
			for (int j = 0; j < smallPic.getHeight(); ++j)
			{
				if (smallPic.at(i, j, 0) > 0 || 
					(smallPic.getColors() == 3 && (smallPic.at(i, j, 1) > 0 || smallPic.at(i, j, 2) > 0)  ) )
				{
					int width = i * downscale;
					int height = j * downscale;
					for (int m = 0; m < downscale; ++m)
					{
						for (int k = 0; k < downscale; ++k)
						{
							for (int c = 0; c < realPic.getColors(); ++c)
							{
								res.at(width + m, height + k, c) = realPic.get(width + m, height + k, c);
							}
						}
					}
				}
			}
		}
	}
	return res;
}
