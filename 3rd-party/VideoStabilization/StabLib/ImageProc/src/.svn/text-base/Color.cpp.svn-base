#include "stdafx.h"
#include "../inc/Color.h"

Color::Color()
{	
	init(0);
}


Color::Color(unsigned char intensity)
{
	init(intensity);
}

void Color::init(unsigned char intensity)
{
	for (int i = 0; i < 3; ++i)
	{
		rgb[i] = intensity;
	}
}


unsigned char& Color::b()
{
	return rgb[0];
}

unsigned char& Color::g()
{
	return rgb[1];
}

unsigned char& Color::r()
{
	return rgb[2];
}
