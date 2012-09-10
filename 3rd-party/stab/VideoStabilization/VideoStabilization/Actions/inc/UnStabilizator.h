#pragma once
#include "IAction.h"
#include "ImageProc/inc/pictureConverter.h"


class UnStabilizator : public IAction
{
	double shiftX, shiftY, shiftAngle;
	PictureConverter picConverter;
	void saveParams() const;
	void loadParams();
public:
	UnStabilizator();
	~UnStabilizator();
	Picture nextStep(Picture pic);

	double getShiftX() const;
	double getShiftY() const;
	double getShiftAngle() const;
	void setShiftXYAngle(double shX, double shY, double shAngle);
};

