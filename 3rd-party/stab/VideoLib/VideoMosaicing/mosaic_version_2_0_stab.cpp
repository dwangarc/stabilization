#include "mosaic_version_2_0_stab.h"
#include "projection.h"
#include "BackwardHM.h"
#include "ForwardHM.h"
#include "inLine.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "libjpeg\jpeglib.h"
#include "libjpeg\jpegutils.h"


Mosaic::Mosaic() : m_Map(NULL),
				m_MapWidth(0),
				m_MapHeight(0),
				m_width(0),
				m_height(0),
				m_MaxX(0),
				m_MaxY(0),
				m_MinX(0),
				m_MinY(0),
				m_LineNumber(15)
{
	m_ProjEngine = new Projection;
	m_ProjEngine->change_iterNum(25);//5, 10
	
	m_Tube = new inLine;
	m_Tube->change_lineNumber(m_LineNumber);
	m_Tube->change_course(10);//3, 5
	m_Tube->change_treshold(20);//5-15

	m_FHM = new ForwardHM;
	m_BHM = new BackwardHM;

	return;
}
Mosaic::~Mosaic()
{
	if(m_Map) delete[] m_Map;
	if(m_ProjEngine) delete m_ProjEngine;
	if(m_Tube) delete m_Tube;
	if(m_FHM) delete m_FHM;
	if(m_BHM) delete m_BHM;
	
	return;
}
int Mosaic::add_frame(unsigned char* im, int im_width, int im_height)
{
	if(!im || im_width <= 0 || im_height <= 0)
		return 0;

	unsigned char* ImGray = convert_to_grayscale(im, im_width, im_height);
	
	if(!m_Map){
		m_width = im_width;
		m_height = im_height;
		m_MapWidth = m_width;
		m_MapHeight = m_height;

		m_Map = new unsigned char[3 * m_MapWidth * m_MapHeight];
		memcpy(m_Map, im, 3 * m_MapWidth * m_MapHeight * sizeof(unsigned char));

		(*m_ProjEngine).load_im(ImGray, m_width, m_height);

		stab_proj[0] = 1.0; stab_proj[1] = 0.0; stab_proj[2] = 0.0;
		stab_proj[3] = 0.0; stab_proj[4] = 1.0; stab_proj[5] = 0.0;
		stab_proj[6] = 0.0; stab_proj[7] = 0.0; stab_proj[8] = 1.0;

		m_Tube->load(ImGray, im, stab_proj, m_width, m_height);

		
		m_MinX = 0;
		m_MinY = 0;
		m_MaxX = m_width - 1;
		m_MaxY = m_height - 1;

		if(ImGray) {delete[] ImGray; ImGray = NULL;}

		return 1;
	}

	if(im_width != m_width || im_height != m_height){
		if(ImGray) {delete[] ImGray; ImGray = NULL;}
		return 0;
	}
	
	m_ProjEngine->load_mask_one(m_FHM->get_history_map());
	ml_save_jpeg_result(m_FHM->get_history_map(), m_width, m_height, "FHM.jpg");
	m_ProjEngine->load_mask_two(m_BHM->get_history_map());
	m_ProjEngine->load_im(ImGray, m_width, m_height);
	
	double pr[9];
	memcpy(pr, (*m_ProjEngine).proj, 8 * sizeof(double));
	pr[8] = 1.0;
	
	double psp[9];
	memcpy(psp, stab_proj, 9 * sizeof(double));
	mat3x3_mult(pr, stab_proj, stab_proj);

	double a0 = (psp[0] != 0) ? 
		((psp[0] - stab_proj[0]) * (psp[0] - stab_proj[0])) / (psp[0] * psp[0]) : 0;
	double a1 = (psp[1] != 0) ? 
		((psp[1] - stab_proj[1]) * (psp[1] - stab_proj[1])) / (psp[1] * psp[1]) : 0;
	double a2 = (psp[2] != 0) ? 
		((psp[2] - stab_proj[2]) * (psp[2] - stab_proj[2])) / (psp[2] * psp[2]) : 0;
	double a3 = (psp[3] != 0) ? 
		((psp[3] - stab_proj[3]) * (psp[3] - stab_proj[3])) / (psp[3] * psp[3]) : 0;
	double a4 = (psp[4] != 0) ? 
		((psp[4] - stab_proj[4]) * (psp[4] - stab_proj[4])) / (psp[4] * psp[4]) : 0;
	double a5 = (psp[5] != 0) ? 
		((psp[5] - stab_proj[5]) * (psp[5] - stab_proj[5])) / (psp[5] * psp[5]) : 0;
	double a6 = (psp[6] != 0) ? 
		((psp[6] - stab_proj[6]) * (psp[6] - stab_proj[6])) / (psp[6] * psp[6]) : 0;
	double a7 = (psp[7] != 0) ? 
		((psp[7] - stab_proj[7]) * (psp[7] - stab_proj[7])) / (psp[7] * psp[7]) : 0;
	double a8 = (psp[8] != 0) ? 
		((psp[8] - stab_proj[8]) * (psp[8] - stab_proj[8])) / (psp[8] * psp[8]) : 0;


	//double treshold = 0.05 * 0.05;
	//if(a0 > treshold || a4 > treshold)
	//	memcpy(stab_proj, psp, 9 * sizeof(double));

	m_Tube->load(ImGray, im, m_ProjEngine->proj, m_width, m_height);
	m_FHM->load(m_Tube->get_diff()[m_LineNumber - 1], 
		m_width, m_height, m_Tube->get_proj()[m_LineNumber - 1]);
	
	m_BHM->load(m_Tube->get_diff(), m_width, m_height, m_LineNumber, 
		m_Tube->get_proj());

	double proec[9];
	memcpy(proec, stab_proj, 9 * sizeof(double));
	mat3x3_invert(proec, proec);

	double xc1 = 0.0;
	double yc1 = 0.0;

	double xc2 = m_width - 1;
	double yc2 = 0;

	double xc3 = 0;
	double yc3 = m_height - 1;

	double xc4 = m_width - 1;
	double yc4 = m_height - 1;

	proj_action(xc1, yc1, stab_proj, m_width, m_height);
	proj_action(xc2, yc2, stab_proj, m_width, m_height);
	proj_action(xc3, yc3, stab_proj, m_width, m_height);
	proj_action(xc4, yc4, stab_proj, m_width, m_height);

	int minX = (__min(__min(int(xc1), int(xc3)), 0));
	int minY = (__min(__min(int(yc1), int(yc2)), 0));
	int maxX = (__max(__max(int(xc2), int(xc4)), m_width - 1));
	int maxY = (__max(__max(int(yc3), int(yc4)), m_height - 1));

	int newX0 = __min(int(xc1), int(xc3));
	int newX1 = __max(int(xc2), int(xc4));
	int newY0 = __min(int(yc1), int(yc2));
	int newY1 = __max(int(yc3), int(yc4));


	int NewMapWidth = maxX - minX + 1;
	int NewMapHeight = maxY - minY + 1;

	unsigned char* NewMap = new unsigned char[3 * NewMapWidth * NewMapHeight];
	memset(NewMap, 0, 3 * NewMapWidth * NewMapHeight * sizeof(unsigned char));

	int shiftX = (m_MinX > minX) ? m_MinX - minX : 0;
	int shiftY = (m_MinY > minY) ? m_MinY - minY : 0;

	for(int j = 0; j < NewMapHeight; j++)
		for(int i = 0; i < NewMapWidth; i++){
			int x = i - shiftX;
			int y = j - shiftY;
			if(x < 0 || x > m_MapWidth - 1 ||
				y < 0 || y > m_MapHeight - 1)
				continue;
			NewMap[3 * (i + j * NewMapWidth) + 0] = 
				m_Map[3 * (x + y * m_MapWidth) + 0];
			NewMap[3 * (i + j * NewMapWidth) + 1] = 
				m_Map[3 * (x + y * m_MapWidth) + 1];
			NewMap[3 * (i + j * NewMapWidth) + 2] = 
				m_Map[3 * (x + y * m_MapWidth) + 2];
		}

	m_MinX = minX;
	m_MinY = minY;
	m_MaxX = maxX;
	m_MaxY = maxY;

	for(int j = newY0; j < newY1; j++)
		for(int i = newX0; i < newX1; i++){
			double x = i;
			double y = j;
			proj_action(x, y, proec, m_width, m_height);
			if(x < 2 || x > m_width - 2 || y < 2 || y > m_height - 2)
				continue;
			int ix = int(x);
			int iy = int(y);

			double dx = x - ix;
			double dy = y - iy;

			int sx = i - minX;
			int sy = j - minY;
			if(sx < 0 || sx > NewMapWidth - 1 ||
				sy < 0 || sy > NewMapHeight - 1)
				return 0;
			for(int c = 0; c < 3; c++){
				NewMap[3 * (sx + sy * NewMapWidth) + c] = unsigned char(
					0.8 * NewMap[3 * (sx + sy * NewMapWidth) + c] + 0.2 * (
					(1.0 - dx) * (1.0 - dy) * im[3 * (ix + iy * m_width) + c] +
					dx * (1.0 - dy) * im[3 * (ix + 1 + iy * m_width) + c] +
					(1.0 - dx) * dy * im[3 * (ix + (iy + 1) * m_width) + c] +
					dx * dy * im[3 * (ix + 1 + (iy + 1) * m_width) + c]));
			}
		}

	if(m_Map) delete[] m_Map;
	m_Map = NewMap;
	m_MapWidth = NewMapWidth;
	m_MapHeight = NewMapHeight;

	if(ImGray) {delete[] ImGray; ImGray = NULL;}

	return 1;
}
unsigned char* Mosaic::convert_to_grayscale(unsigned char* im, int im_width, int im_height)
{
	unsigned char* ImGray = new unsigned char[im_width * im_height];
	memset(ImGray, 0, im_width * im_height * sizeof(unsigned char));
	unsigned char* src = im;
	unsigned char* dst = ImGray;
	for(int j = 0; j < im_height; j++)
		for(int i = 0; i < im_width; i++){
			dst[0] = static_cast<unsigned char>((src[0] + src[1] + src[2]) / 3.0);
			//	(0.212671 * src[0] + 0.715160 * src[1] + 0.072169 * src[2]);
			//0.212671 * R + 0.715160 * G + 0.072169 * B;
			src += 3;
			dst++;
		}
	return ImGray;
}
int Mosaic::mat3x3_invert(double *m, double *m_inv)
{
	double res[9];
	double d = m[0]*(m[4]*m[8] - m[5]*m[7]) - m[1]*(m[3]*m[8] - m[5]*m[6]) + m[2]*(m[3]*m[7] - m[4]*m[6]);
	
	if (d == 0) return 0;
	
	d = 1 / d;

	res[0] = (m[4] * m[8] - m[5] * m[7]) * d;
	res[1] = (m[2] * m[7] - m[1] * m[8]) * d;
	res[2] = (m[1] * m[5] - m[2] * m[4]) * d;
                                  
	res[3] = (m[5] * m[6] - m[3] * m[8]) * d;
	res[4] = (m[0] * m[8] - m[2] * m[6]) * d;
	res[5] = (m[2] * m[3] - m[0] * m[5]) * d;
                                  
	res[6] = (m[3] * m[7] - m[4] * m[6]) * d;
	res[7] = (m[1] * m[6] - m[0] * m[7]) * d;
	res[8] = (m[0] * m[4] - m[1] * m[3]) * d;

	memcpy(m_inv, res, 9 * sizeof(double));

	return 1;
}
int Mosaic::proj_action(double &x, double &y, double* proj, int width, int height)
{
	double xt = x - width * 0.5;
	double yt = y - height * 0.5;

	double denom = proj[6] * xt + proj[7] * yt + proj[8];
	if(!denom) return 0;

	x = proj[0] * xt + proj[1] * yt + proj[2];
	y = proj[3] * xt + proj[4] * yt + proj[5];

	denom = 1.0 / denom;

	x *= denom;
	y *= denom;

	x += width * 0.5;
	y += height * 0.5;

	return 1;
}
int Mosaic::mat3x3_mult(double *m_left, double *m_right, double *m_result)
{
	double res[9];
	res[0] = m_left[0] * m_right[0] + m_left[1] * m_right[3] + m_left[2] * m_right[6];
	res[1] = m_left[0] * m_right[1] + m_left[1] * m_right[4] + m_left[2] * m_right[7];
	res[2] = m_left[0] * m_right[2] + m_left[1] * m_right[5] + m_left[2] * m_right[8];
	res[3] = m_left[3] * m_right[0] + m_left[4] * m_right[3] + m_left[5] * m_right[6];
	res[4] = m_left[3] * m_right[1] + m_left[4] * m_right[4] + m_left[5] * m_right[7];
	res[5] = m_left[3] * m_right[2] + m_left[4] * m_right[5] + m_left[5] * m_right[8];
	res[6] = m_left[6] * m_right[0] + m_left[7] * m_right[3] + m_left[8] * m_right[6];
	res[7] = m_left[6] * m_right[1] + m_left[7] * m_right[4] + m_left[8] * m_right[7];
	res[8] = m_left[6] * m_right[2] + m_left[7] * m_right[5] + m_left[8] * m_right[8];

	memcpy(m_result, res, 9 * sizeof(double));

	return 1;
}
int Mosaic::ml_save_jpeg_result_RGB(unsigned char* im, int width, int height, const char* name)
{
	if(!im || !width || !height || !name)
		return 0;
	//::WaitForSingleObject(frm->libjpeg_thread, INFINITE);
		jpeg_compress_struct*		cinfo = new jpeg_compress_struct;
		jpeg_error_mgr*				jerr = new jpeg_error_mgr;
		cinfo->err = jpeg_std_error(jerr);
		jpeg_create_compress(cinfo);

		FILE * outfile;
		if(fopen_s(&outfile, name, "wb"))
			return 0;
	
		jpeg_stdio_dest(cinfo, outfile);
		cinfo->image_width = width; 
		cinfo->image_height = height;
		cinfo->input_components = 3;
		cinfo->in_color_space = JCS_RGB; 
		jpeg_set_defaults(cinfo);
		jpeg_start_compress(cinfo, TRUE);
	
		JSAMPROW row_pointer[1];
		JSAMPROW image_buffer = (JSAMPROW)im;
		int row_stride = 3 * width;

		while (cinfo->next_scanline < cinfo->image_height) {
			row_pointer[0] = &image_buffer[cinfo->next_scanline * row_stride];
			jpeg_write_scanlines(cinfo, row_pointer, 1);
		}
		jpeg_finish_compress(cinfo);
		jpeg_destroy_compress(cinfo);
		fclose(outfile);
		
		if(cinfo)	delete cinfo;
		if(jerr)	delete jerr;
	
	//::SetEvent(frm->libjpeg_thread);

	return 1;
}
int Mosaic::ml_save_jpeg_result(unsigned char* im, int width, int height, const char* name)
{
	if(!im || !width || !height || !name)
		return 0;
	//::WaitForSingleObject(frm->libjpeg_thread, INFINITE);
		jpeg_compress_struct*		cinfo = new jpeg_compress_struct;
		jpeg_error_mgr*				jerr = new jpeg_error_mgr;
		cinfo->err = jpeg_std_error(jerr);
		jpeg_create_compress(cinfo);

		FILE * outfile;
		if(fopen_s(&outfile, name, "wb"))
			return 0;
	
		jpeg_stdio_dest(cinfo, outfile);
		cinfo->image_width = width; 
		cinfo->image_height = height;
		cinfo->input_components = 1;
		cinfo->in_color_space = JCS_GRAYSCALE; 
		jpeg_set_defaults(cinfo);
		jpeg_start_compress(cinfo, TRUE);
	
		JSAMPROW row_pointer[1];
		JSAMPROW image_buffer = (JSAMPROW)im;
		int row_stride = width;

		while (cinfo->next_scanline < cinfo->image_height) {
			row_pointer[0] = &image_buffer[cinfo->next_scanline * row_stride];
			jpeg_write_scanlines(cinfo, row_pointer, 1);
		}
		jpeg_finish_compress(cinfo);
		jpeg_destroy_compress(cinfo);
		fclose(outfile);
		
		if(cinfo)	delete cinfo;
		if(jerr)	delete jerr;
	
	//::SetEvent(frm->libjpeg_thread);

	return 1;
}


