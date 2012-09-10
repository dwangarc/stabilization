#include "stdafx.h"
#include <stdio.h>
#include "libjpeg\jpeglib.h"
#include "libjpeg\jpegutils.h"
#include "MainFrm.h"
#include "VideoMosaic.h"
#include <math.h>
#include <stdlib.h>
#include <memory.h>

#include "mosaic_Version_2_0_stab.h"

int ml_save_jpeg_result(unsigned char* im, int width, int height, const char* name);
int ml_save_jpeg_result_RGB(unsigned char* im, int width, int height, const char* name);
int Scale(unsigned char* &im, int SrcWidth, int SrcHeight, int TgtWidth, int TgtHeight);
void ScaleLine(unsigned char *Target, unsigned char* Source, int SrcWidth, int TgtWidth);
unsigned char*	ml_convert_to_grayscale(unsigned char* im, int width, int height);

typedef struct ml_track_data {
	
	int				width;
	int				height;
	track_target	target;

	Mosaic* mos;

} ml_track_data;



ml_track_data* ml_track_create(int width, int height)
{
	ml_track_data *data = new ml_track_data;
	if (!data) return 0;
	memset(data, 0, sizeof(ml_track_data));
	data->width = width;
	data->height = height;
	data->mos = new Mosaic;

	return data;
}
int	ml_track_init(ml_track_data *data, unsigned char *im, int step, track_target *target)
{
	data->target = *target;
	int width = data->width;
	int height = data->height;

	data->mos->add_frame(im, width, height);

	return 1;
}
double	ml_track_track(ml_track_data *data, unsigned char* &im, int step, 
				   track_target *target)
{
	static int frame_num = 0;
	if(!frame_num)
	{frame_num++; return 1;}
	
	frame_num++;
	int width = data->width;
	int height = data->height;

	data->mos->add_frame(im, width, height);
	int MapWidth = data->mos->m_MapWidth;
	int MapHeight = data->mos->m_MapHeight;
	unsigned char* temp = new unsigned char[3 * MapWidth * MapHeight];
	memset(temp, 0, 3 * MapWidth * MapHeight * sizeof(unsigned char));
	memcpy(temp, data->mos->m_Map, 3 * MapWidth * MapHeight * sizeof(unsigned char));
	Scale(temp, MapWidth, MapHeight, width, height);
	memcpy(im, temp, 3 * width * height * sizeof(unsigned char));
	delete[] temp;

	
	
	target->x = data->target.x;
	target->y = data->target.y;
	target->w = data->target.w;
	target->h = data->target.h;

	return 1;
}
void ml_track_release(ml_track_data *data)
{
	if (!data) return;
	
	delete data->mos;
	delete data;
}
unsigned char* ml_convert_to_grayscale(unsigned char* im, int width, int height)
{
	unsigned char* ImGray = new unsigned char[width*height];
	memset(ImGray, 0, width*height*sizeof(unsigned char));
	unsigned char* src = im;
	unsigned char* dst = ImGray;
	for(int j = 0; j < height; j++)
		for(int i = 0; i < width; i++){
			dst[0] = (src[0] + src[1] + src[2]) / 3;
			src += 3;
			dst++;
		}
	return ImGray;
}
int ml_save_jpeg_result(unsigned char* im, int width, int height, const char* name)
{
	CMainFrame *frm = (CMainFrame*)theApp.m_pMainWnd;

	if(!im || !width || !height || !name)
		return 0;
	::WaitForSingleObject(frm->libjpeg_thread, INFINITE);
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
	
	::SetEvent(frm->libjpeg_thread);

	return 1;
}
int ml_save_jpeg_result_RGB(unsigned char* im, int width, int height, const char* name)
{
	CMainFrame *frm = (CMainFrame*)theApp.m_pMainWnd;

	if(!im || !width || !height || !name)
		return 0;
	::WaitForSingleObject(frm->libjpeg_thread, INFINITE);
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
	
	::SetEvent(frm->libjpeg_thread);

	return 1;
}
int Scale(unsigned char* &im, int SrcWidth, int SrcHeight, int TgtWidth, int TgtHeight)
{
	if(!im || SrcWidth <= 0 || SrcHeight <= 0 || TgtWidth <= 0 || TgtHeight <= 0)
		return 0;
	
	unsigned char* Target = new unsigned char[3 * TgtWidth * TgtHeight];
	memset(Target, 0, 3 * TgtWidth * TgtHeight * sizeof(unsigned char));
	unsigned char* Source = im;
	unsigned char* TgtTemp = Target;
	unsigned char* SrcTemp = im;

	int NumPixels = TgtHeight;
	int IntPart = (SrcHeight / TgtHeight) * SrcWidth;
	int FractPart = SrcHeight % TgtHeight;
	int E = 0;
	unsigned char* PrevSource = NULL;

	while (NumPixels-- > 0){
		if(Source == PrevSource){
			memcpy(Target, Target - 3 * TgtWidth, 3 * TgtWidth * sizeof(*Target));
		} else{
			ScaleLine(Target, Source, SrcWidth, TgtWidth);
			PrevSource = Source;
		}
		Target += 3 * TgtWidth;
		Source += 3 * IntPart;
		E += FractPart;
		if(E >= TgtHeight){
			E -= TgtHeight;
			Source += 3 * SrcWidth;
		}
	}

	Target = TgtTemp;
	Source = SrcTemp;

	delete[] im;
	SrcWidth = TgtWidth;
	SrcHeight = TgtHeight;
	im = new unsigned char[3 * SrcWidth * SrcHeight * sizeof(unsigned char)];
	memcpy(im, Target, 3 * SrcWidth * SrcHeight * sizeof(unsigned char));

	delete[] Target;

	return 1;
}
void ScaleLine(unsigned char *Target, unsigned char* Source, int SrcWidth, int TgtWidth)
{
	int NumPixels = TgtWidth;
	int IntPart = SrcWidth / TgtWidth;
	int FractPart = SrcWidth % TgtWidth;
	int E = 0;
	unsigned char pR = 0;
	unsigned char pG = 0;
	unsigned char pB = 0;

	while (NumPixels-- > 0){
		pR = Source[0];
		pG = Source[1];
		pB = Source[2];

		Target[0] = pR;
		Target[1] = pG;
		Target[2] = pB;

		Target += 3;
		Source += 3 * IntPart;
		E += FractPart;
		if (E >= TgtWidth){
			E -= TgtWidth;
			Source += 3;
		}
	}
	return;
}