#pragma once
#include <string>
#include <sstream>
#include <WTypes.h>
#include <sys\types.h>
#include <sys\stat.h>
#include "image.h"

typedef boost::filesystem::path  b_path;


inline std::string sztToStr(size_t val)
{
	std::stringstream out;
	out << val;
	std::string st = out.str();
	return st;
}

inline std::string addLeadingZeros(std::string st, size_t length)
{
	if (st.length() < length)
	{
		st.insert(0, length - st.length(), '0');
	}
	return st;
}

inline __int64 FileSize64 (const char* szFileName)
{
	struct __stat64 fileStat;
	int err = _stat64(szFileName, &fileStat);
	if (0 != err) return 0;
	return fileStat.st_size;
}


class FramesReader
{
	static const int MIN_FILE_SIZE = 200;
	std::string prefix, extention;
	std::string outPrefix;
	int outNum;
	size_t numSize, curFrame;
	DWORD maxWait, sleepTime;
	size_t maxFrameNum;
	std::string getName(size_t name);
	bitmap* latest;
public:
	/*
	image00013.jpeg
	prefix == image
	exteniton == jpeg
	numberSize == 5
	*/
	FramesReader(std::string const& prefix, std::string const& extention, size_t numberSize, DWORD sleepTime = 5, DWORD maxWaitTime = 5000, size_t firstFrame = 1);
	~FramesReader();
	bitmap* nextFrame();
	void saveFrame(bitmap*);
	void saveFrame(unsigned char const* pic, int width, int height, int colors = 1, char const* fileName = 0);
};