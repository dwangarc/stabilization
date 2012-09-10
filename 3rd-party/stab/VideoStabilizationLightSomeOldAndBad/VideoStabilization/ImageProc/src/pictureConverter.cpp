#include "stdafx.h"
#include "../inc/pictureConverter.h"

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


Picture PictureConverter::convertColor(Picture pic) const
{
	int width = pic.getWidth();
	int height = pic.getHeight();
	int dim = pic.getColors();
	Picture res(width, height);
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
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			Point2D xy(j, i);
			Point2D xy1 = homo.applyHomo(xy);
			for (int c = 0; c < colors; ++c)
			{
				res.at(j, i, c) = (unsigned char)pic.getColor(xy1.x, xy1.y, c);
			}
		}
	}
	return res;

}

Picture PictureConverter::downscale(Picture pic, int downscale) const
{
	int colors = pic.getColors();
	int widthD = pic.getWidth() / downscale;
	int heightD = pic.getHeight() / downscale;
	Picture res(widthD, heightD, colors);
	for (int c = 0; c < colors; ++c)
	{
		for (int i = 0; i < heightD; ++i)
		{
			for (int j = 0; j < widthD; ++j)
			{
				int sum = 0;
				for (int g = 0; g < downscale; ++g)
				{
					for (int k = 0; k < downscale; ++k)
					{
						sum += pic.get(j * downscale + k, i * downscale + g, c);
					}
				}
				sum /= downscale * downscale;
				res.at(j, i, c) = (unsigned char)sum;
			}
		}
	}
	return res;
}


Picture PictureConverter::unFixDistortions(StereoConverter& converter, Picture pic)
{
	Picture res(pic.getWidth(), pic.getHeight(), pic.getColors());
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
					res.at((int)shift.x, (int)shift.y, c) = (unsigned char) pic.getColor((int)shift.x, (int)shift.y, c);
				}
			}
		}
	}
	return res;
}

Picture PictureConverter::fixDistortions(StereoConverter& converter, Picture pic)
{
	Picture res(pic.getWidth(), pic.getHeight(), pic.getColors());
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
					res.at(i, j, c) = (unsigned char)pic.getColor((int)shift.x, (int)shift.y, c);
				}
			}
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
	for (int c = 0; c < colors; ++c)
	{
		for (int i = 0; i < heightD; ++i)
		{
			for (int j = 0; j < widthD; ++j)
			{
				int vl1 = pic.at(j / 2, i / 2, c);
				int vl2 = pic.at(j / 2 + j % 2, i / 2 + i % 2, c);
				res.at(j, i, c) = (vl1 + vl2) / 2;
			}
		}
	}
	return res;
}
