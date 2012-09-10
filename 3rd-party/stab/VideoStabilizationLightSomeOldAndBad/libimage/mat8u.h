#include "ImageInfo.h"

#pragma once


//TODO: ј где PNG?
class JpgMat8u;
class BmpMat8u;
class TifMat8u;

class mat8u {
protected:
	//mat8u() {} ;
	//mat8u(boost::filesystem::path & aPath);

	unsigned char *data;
	int width, height, step;

public:

	~mat8u();
	//mat8u(const mat8u & aMap8u);

	unsigned char * getData();
	int getWidth();
	int getHeight();
	int getStep();
	void setDataPointer(unsigned char *newdata);

	// пам€ть под data нужно выделить извне и передать указатель в createByPath в качестве параметра
	static boost::shared_ptr<mat8u> createByPath(boost::filesystem::path & aPath, unsigned char *img_data)
	{
		std::string uppercaseExtension(aPath.extension());
		std::transform(uppercaseExtension.begin(), uppercaseExtension.end(), uppercaseExtension.begin(), toupper);

		static bool isInitialized = false;
		if (!isInitialized)
		{
			isInitialized = true;
			RegisterInFactory<mat8u, JpgMat8u> jpg(".JPG");
			RegisterInFactory<mat8u, BmpMat8u> bmp(".BMP");
			RegisterInFactory<mat8u, TifMat8u> tif(".TIF");
		}

		boost::shared_ptr<mat8u> result = GenericFactory<mat8u>::getInstance().create(uppercaseExtension);
		result->setDataPointer(img_data);
		result->initialize(aPath);
		return result;
	}

	virtual boost::shared_ptr<mat8u> copy() = 0;
	virtual void load(boost::filesystem::path & aPath) = 0;
private:

	void initialize(boost::filesystem::path & aPath);
	void allocateMemory(WidthAndHeight & aWidthAndHeight);
};

class ScannerMat8u: public mat8u
{
public:
	ScannerMat8u() {};
	ScannerMat8u(const ScannerMat8u & aMap8u);

	boost::shared_ptr<mat8u> copy();
	void load(boost::filesystem::path & aPath);
};

class JpgMat8u: public mat8u
{
public:
	JpgMat8u() {};
	JpgMat8u(const JpgMat8u & aMap8u);

	boost::shared_ptr<mat8u> copy();
	void load(boost::filesystem::path & aPath);
};

class BmpMat8u: public mat8u
{
public:
	BmpMat8u() {};
	BmpMat8u(const BmpMat8u & aMap8u);

	boost::shared_ptr<mat8u> copy();
	void load(boost::filesystem::path & aPath);
};

class TifMat8u: public mat8u
{
public:
	TifMat8u() {};
	TifMat8u(const TifMat8u & aMap8u);

	boost::shared_ptr<mat8u> copy();
	void load(boost::filesystem::path & aPath);
};