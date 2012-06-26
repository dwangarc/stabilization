#pragma once
#include "Picture.h"
#include "Include/define.h"

class PictureUpgrader
{
public:
	sh_ptr_int gistogramEstimator(Picture pic, int bins) const;
	Picture gistogramAlign(Picture pic) const;
};