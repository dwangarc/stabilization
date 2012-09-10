#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <libimage/GenericFactory.h>

struct WidthAndHeight 
{
	int width;
	int height;
};

class JpgImageInfo;
class PngImageInfo;
class BmpImageInfo;
class TifImageInfo;

class ImageInfoException: std::runtime_error
{
public:
	ImageInfoException(const char * aWhat) : std::runtime_error(aWhat)
	{
	}

	static void throwIf(bool aCondition, const char * aWhat)
	{
		if (aCondition == true)
		{
			throw new ImageInfoException(aWhat);
		}
	}
};

class ImageInfo
{
public:
	//���� ����� ���������� ��������� ���������������� ���������,
	//������� ����� ��� ���� ������ � ����� � ��� ��������, ����..
	//���������� ����� ��� RegisterInFactory - ��������� �����,
	//��� ������� - �������������� � ��������������� GenericFactory
	//������� �������� �������� �� ������������������� ����������.
	static boost::shared_ptr<ImageInfo> createByExtension(std::string & aString)
	{
		std::string uppercaseExtension(aString);
		std::transform(uppercaseExtension.begin(), uppercaseExtension.end(), uppercaseExtension.begin(), toupper);
		
		static bool isInitialized = false;
		if (!isInitialized)
		{
			isInitialized = true;
			RegisterInFactory<ImageInfo, JpgImageInfo> jpg(".JPG");
			RegisterInFactory<ImageInfo, PngImageInfo> png(".PNG");
			RegisterInFactory<ImageInfo, BmpImageInfo> bmp(".BMP");
			RegisterInFactory<ImageInfo, TifImageInfo> tif(".TIF");
		}

		return GenericFactory<ImageInfo>::getInstance().create(uppercaseExtension);
	}

public:
	virtual WidthAndHeight getSize(boost::filesystem::path & aPath) = 0;
};

class JpgImageInfo: public ImageInfo
{
public:
	WidthAndHeight getSize(boost::filesystem::path & aPath);
};

class PngImageInfo: public ImageInfo
{
public:
	WidthAndHeight getSize(boost::filesystem::path & aPath);
};

class BmpImageInfo: public ImageInfo
{
public:
	WidthAndHeight getSize(boost::filesystem::path & aPath);
};

class TifImageInfo: public ImageInfo
{
public:
	WidthAndHeight getSize(boost::filesystem::path & aPath);
};
