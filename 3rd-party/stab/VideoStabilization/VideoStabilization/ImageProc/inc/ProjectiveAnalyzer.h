#pragma once

class ProjectiveAnalyzer
{
	int analyzeIter;
	double medX, medY, medAngle;

public:
	ProjectiveAnalyzer();
	void reset();
	void nextStep(double const* proj);

	double getMedX() const;
	double getMedY() const;
	double getMedAngle() const;

};