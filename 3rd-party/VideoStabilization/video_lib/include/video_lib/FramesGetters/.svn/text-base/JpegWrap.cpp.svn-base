#include "stdafx.h"
#include "JpegWrap.h"

#include <iostream>
#include <fstream>

JpegWrap::JpegWrap(unsigned char const* data, int length)
{
	this->length = length;
	pic = sh_ptr_uch(new unsigned char[length]);
	memcpy(pic.get(), data, length * sizeof(unsigned char));
}

void JpegWrap::saveFrame(const char* fileName)
{
	std::fstream file(fileName, std::ios_base::out| std::ios_base::binary);
	file.write((const char*)pic.get(), length);
	file.close();
}
