#pragma once
#include <string>
#include <sstream>
#include <WTypes.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <list>
//#include "image.h"
#include "Include/FrameGetterInterface.h"

#include <boost/filesystem.hpp>


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


class FramesReader : public IFrameGetter
{
	//std::string outPrefix, outExtention;
	//int outNum, numSize;
	//bitmap* latest;
	std::list<std::string> selectedFramesNames;
public:
	/*
	image00013.jpeg
	prefix == image
	exteniton == jpeg
	numberSize == 5
	*/
	FramesReader(/*std::string const& outPrefix, std::string const& outExtention, int numberSize*/);
	virtual ~FramesReader();
	
//	bitmap const* nextSelectedFrame();
	Picture nextPic();
//	Picture nextSelectedFramePic();
	void addSelectedFrame(std::string const& frameName);
	void clearSelectedFrames();

//	void saveFrame(bitmap*);
//	void saveFrame(Picture pic, char const* fileName/* = 0*/);

};