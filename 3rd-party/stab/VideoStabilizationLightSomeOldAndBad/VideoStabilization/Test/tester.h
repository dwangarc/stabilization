#pragma once

class Tester
{
	int width, height;
public:
	void setWH(int width, int height);
	double imgDistace(unsigned char const* pic1, unsigned char const* pic2, int colors) const;
};