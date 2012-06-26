#pragma once

#include "ImageProc/inc/Picture.h"

struct IFrameGetter
{
	virtual Picture nextPic() = 0;
	virtual ~IFrameGetter(){};

	virtual bool isStarted() const
	{
		return true;
	}

	virtual bool start() 
	{
		return true;
	}

	virtual void stop()
	{

	}
};