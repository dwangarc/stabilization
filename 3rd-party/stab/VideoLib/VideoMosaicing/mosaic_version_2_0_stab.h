#pragma once

class Projection;
class inLine;
class ForwardHM;
class BackwardHM;

class Mosaic
{
public:

	Mosaic();
	~Mosaic();

	int add_frame(unsigned char* im, int im_width, int im_height);
	int mat3x3_mult(double *m_left, double *m_right, double *m_result);


public:

	unsigned char* m_Map;
	int m_MapWidth;
	int m_MapHeight;

private:

	unsigned char* convert_to_grayscale(unsigned char* im, int im_width, int im_height);
	int mat3x3_invert(double *m, double *m_inv);
	int proj_action(double &x, double &y, double* proj, int width, int height);
	int ml_save_jpeg_result_RGB(unsigned char* im, int width, int height, const char* name);
	int ml_save_jpeg_result(unsigned char* im, int width, int height, const char* name);


	
private:

	int m_width;
	int m_height;

	Projection* m_ProjEngine;
	inLine* m_Tube;
	ForwardHM* m_FHM;
	BackwardHM* m_BHM;
	
	double stab_proj[9];

	int m_MinX;
	int m_MinY;
	int m_MaxX;
	int m_MaxY;

	int m_LineNumber;
};