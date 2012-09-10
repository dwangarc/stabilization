#pragma once

struct Color
{
	unsigned char rgb[3];

	Color();
	Color(unsigned char intensity);

	void init(unsigned char intensity);

	unsigned char& r();
	unsigned char& g();
	unsigned char& b();

};
