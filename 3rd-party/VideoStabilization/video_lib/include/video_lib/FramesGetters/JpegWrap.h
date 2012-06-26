#pragma once

#include "boost/shared_ptr.hpp"

typedef boost::shared_ptr<unsigned char> sh_ptr_uch;

struct JpegWrap
{
	int length;
	sh_ptr_uch pic;

	JpegWrap(unsigned char const* data, int length);
	void saveFrame(const char* fileName);
};