#include "projection.h"
#include <math.h>
//#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <omp.h>

#include "Include/define.h"
#include "ProjectiveConverter.h"
//#include <assert.h>
//#include <float.h>

//SVD to solve the equation (include svd.cpp)
void	SVD(double* a, int lda, int m, int n, double* w, double* uT, 
			int lduT, double* vT, int ldvT, double* buffer);
void	SVBkSb(int m, int n, double* w, double* uT, int lduT, double* vT, 
			int ldvT, double* b, int ldb, int nb, double* x, int ldx, double* buffer);	

void Projection::initProj(double* proj1, bool nine)
{
	proj1[0] = 1.0; proj1[1] = 0.0; proj1[2] = 0.0;
	proj1[3] = 0.0; proj1[4] = 1.0; proj1[5] = 0.0;
	proj1[6] = 0.0; proj1[7] = 0.0;
	if (nine)
	{
		proj1[8] = 1.0;
	}
}

int Projection::getWidth() const
{
	return width;
}

int Projection::getHeight() const
{
	return height;
}

Projection::Projection() 
{
	initProj(proj);
	

	im1 = NULL;
	im2 = NULL;

	width = 0;
	height = 0;

	gauss_depth = 1;
	iterNum = 10;


	tau = 0.07;

	Ex = NULL;
	Ey = NULL;
	Et = NULL;
	proj_picture = NULL;
	memset(A, 0, 64 * sizeof(double));
	memset(C, 0, 8 * sizeof(double));
	memcpy(q, proj, 8 * sizeof(double));
}
Projection::~Projection()
{
/*	if(im1)				{delete[] im1; im1 = NULL;}
	if(im2)				{delete[] im2; im2 = NULL;}*/
	if(Ex)				{delete[] Ex; Ex = NULL;}
	if(Ey)				{delete[] Ey; Ey = NULL;}
	if(Et)				{delete[] Et; Et = NULL;}
	if(proj_picture)	{delete[] proj_picture; proj_picture = NULL;}
//	if (filter)			{delete[] filter; filter = 0;}
//	if(stab_picture)	{delete[] stab_picture; stab_picture = NULL;}
	//if(mask_one)		{delete[] mask_one; mask_one = NULL;}
	//if(mask_two)		{delete[] mask_two; mask_two = NULL;}
	return;
}


void Projection::initNonPic(int im_width, int im_height)
{
	clear_data();
	width = im_width;
	height = im_height;
	proj_picture = new unsigned char[width * height];
	memset(proj_picture, 0, width * height * sizeof(unsigned char));
	Ex = new int[width * height];
	Ey = new int[width * height];
	Et = new int[width * height];

}

int Projection::estimateProj(unsigned char* image1, unsigned char* image2, int iterNumber, 
							 double const* proj1)
{
	im1 = image1;
	im2 = image2;
	if (proj1)
	{
		memcpy(proj, proj1, 8 * sizeof(double));
	}
	else
	{
		initProj(proj);
	}

	prepare_proj_picture();
	for (int iter = 1; iter <= iterNumber; ++iter) 
	{
		calculate_maps(iter);
		calculate_matrix(iter);

		calculate_q();
		calculate_proj();

		if (iter != iterNumber)
		{
			prepare_proj_picture();
		}
	}

	return 1;
}


int Projection::gauss_filter(unsigned char* im, int im_width, int im_height) const
{
	if(!im || !im_width || !im_height)
		return 0;
	int* s = new int[(im_width - 4) * (im_height - 4)];

	//dxx[1] = im; dxx[2] = im + im_width;
	//dxx[3] = im + 2 * im_width; dxx[4] = im + 3 * im_width;

#pragma omp parallel for schedule(static)
	for(int j = 2; j < im_height - 2; ++j)
	{
		unsigned char* dxx[5];
		int* gxx = new int[im_width];
		int* Gxx = new int[im_width - 4];
		for (int i = 0; i < 5; ++i)
		{
			dxx[i] = im + (i + j - 2) * im_width;
		}

		for(int i = 0; i < im_width; ++i)
		{
			gxx[i] = dxx[0][i] + (dxx[1][i] << 2) + (dxx[2][i] << 2) + (dxx[2][i] << 1) + (dxx[3][i] << 2) + dxx[4][i];
		}
		for(int i = 0; i < im_width - 4; ++i)
		{
			Gxx[i] = gxx[i] + (gxx[i + 1] << 2) + (gxx[i + 2] << 2) + (gxx[i + 2] << 1) + (gxx[i + 3] << 2) + gxx[i + 4];
		}
		int offset = (j - 2) * (im_width - 4);
		for(int j = 0; j < im_width - 4; ++j)
		{
			s[j + offset] = Gxx[j] >> 8;
		}

		delete [] gxx;
		delete [] Gxx;

		//gauss_sweep_one(dxx, im_width, gxx);
		//gauss_sweep_two(gxx, im_width, Gxx);
		//gauss_sweep_three(Gxx, im_width, s + (j - 2) * (im_width - 4));
	}

	gauss_sweep_four(s, im_width, im_height, im);
	delete [] s;
	return 1;
}
int Projection::gauss_sweep_one(unsigned char** dxx, int im_width, int* gxx) const
{
	for(int i = 0; i < im_width; i++)
	{
		gxx[i] = dxx[0][i] + (dxx[1][i] << 2) + (dxx[2][i] << 2) + (dxx[2][i] << 1) + (dxx[3][i] << 2) + dxx[4][i];
	}
	return 1;	
}
int Projection::gauss_sweep_two(int* gxx, int im_width, int* Gxx) const
{
	for(int i = 0; i < im_width - 4; i++)
	{
		Gxx[i] = gxx[i] + (gxx[i + 1] << 2) + (gxx[i + 2] << 2) + (gxx[i + 2] << 1) + (gxx[i + 3] << 2) + gxx[i + 4];
	}
	return 1;
}
int Projection::gauss_sweep_three(int* Gxx, int im_width, int* s) const
{
	for(int j = 0; j < im_width - 4; j++)
		s[j] = Gxx[j] >> 8;

	return 1;
}
int	Projection::gauss_sweep_four(int * s, int im_width, int im_height, unsigned char* im) const
{
	memset(im, 0, im_width * im_height * sizeof(unsigned char));

#pragma omp parallel for schedule(static)
	for(int j = 2; j < im_height - 2; j++)
	{
		int jWidth = j * im_width;
		int j2Width4 = (j - 2) * (im_width - 4);
		for(int i = 2; i < im_width - 2; i++)
		{
			im[i + jWidth] = s[i - 2 + j2Width4];
		}
	}

	return 1;
}

double Projection::calculateDerX(int x, int y) const
{
	double der = 0;
	for (int i = -1; i < 2; ++i)
	{
		der += im2[(y + i) * width + x + 1];
		der -= im2[(y + i) * width + x - 1];
	}
	der += im2[y * width + x + 1];
	der -= im2[y * width + x - 1];
	der /= 8;
	return der;
}

double Projection::calculateDerY(int x, int y) const
{
	double der = 0;
	for (int i = -1; i < 2; ++i)
	{
		der += im2[(y + 1) * width + x + i];
		der -= im2[(y - 1) * width + x + i];
	}
	der += im2[(y + 1) * width + x];
	der -= im2[(y - 1) * width + x];
	der /= 8;
	return der;

}



int Projection::calculate_maps(int iteration)
{
	if(!im1 || !im2 || !Ex || !Ey || !Et || !proj_picture || !width || !height)
		return 0;
		
	int bandWidth = 16;
	if(iteration == 1) 
	{
		memset(Ex, 0, width * height * sizeof(int));
		memset(Ey, 0, width * height * sizeof(int));  
		memset(Et, 0, width * height * sizeof(int));
		#pragma omp parallel
		{
			#pragma omp for schedule(static)
			for (int j = bandWidth; j < height - bandWidth; ++j)
			{
				int jwidth = j * width;
				int place = bandWidth + jwidth;
				for (int i = bandWidth; i < width - bandWidth; ++i) 
				{
					Et[place] = im1[place] - im2[place];
					Ex[place] = /*calculateDerX(i, j);*/(im2[place + 1] - im2[place - 1])/* / 2.0*/;
					Ey[place] = /*calculateDerY(i, j);*/(im2[place + width] - im2[place - width])/* / 2.0*/;
					++place;
				}
			}
		}
	}
	else 
	{
		memset(Et, 0, width * height * sizeof(int));
		#pragma omp parallel
		{
			#pragma omp for	schedule(dynamic, 16)
			for (int j = bandWidth; j < height - bandWidth; ++j)
			{
				int jwidth = j * width;
				int place = bandWidth + jwidth;
				for (int i = bandWidth; i < width - bandWidth; ++i)
				{
					if(!proj_picture[place])
					{
						Et[place] = 0;
					}
					else
					{
						Et[place] = proj_picture[place] - im2[place];
					}
					++place;
				}
			}
		}
	}

	return 1;
}
int Projection::calculate_matrix(int iteration)
{
	if(!Ex || !Ey || !Et || !width || !height) 
		return 0;

	int shift = width / 20;
	int width2 = width / 2;
	int height2 = height / 2;
	if(iteration == 1)
	{
		memset(A, 0, 64 * sizeof(double));
		memset(C, 0, 8 * sizeof(double));
		#pragma omp parallel
		{
			double t_C[8] = { 0 };
			double t_A[64] = { 0 };

			#pragma omp for schedule (static)
			for (int j = shift; j < height - shift; ++j)
			{
				double y = j - height2;
				double yy = y * y;
				int c = shift + j * width;
				int iMinWidth2 = shift - width2;

				for (int i = shift; i < width - shift; ++i, ++c, ++iMinWidth2) 
				{
					double x = iMinWidth2;

					double xx = x * x;
					double xy = x * y;
					double EXX = Ex[c] * Ex[c];
					double EYY = Ey[c] * Ey[c];
					double EXY = Ex[c] * Ey[c];
					double ExEt = Ex[c] * Et[c];
					double EyEt = Ey[c] * Et[c];

					double EXXxx = EXX * xx;
					double EXXxy = EXX * xy;
					double EXYxx = EXY * xx;
					double EXYxy = EXY * xy;
					double EXYyy = EXY * yy;
					double EYYxy = EYY * xy;
					double EYYyy = EYY * yy;


					double EXxxVsEYxy = xx * Ex[c] + xy * Ey[c];
					double EXxyVsEYyy = xy * Ex[c] + yy * Ey[c];


					t_A[0] += EXXxx;//Ex[c] * x * Ex[c] * x;
					t_A[1] += EXXxy;//Ex[c] * x * Ex[c] * y;
					t_A[2] += EXX * x; //Ex[c] * x * Ex[c];
					t_A[3] += EXYxx;//Ex[c] * x * Ey[c] * x;
					t_A[4] += EXYxy;//Ex[c] * x * Ey[c] * y;
					t_A[5] += EXY * x; //Ex[c] * x * Ey[c];
					t_A[6] += (EXXxx + EXYxy) * x;// Ex[c] * x * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[7] += (EXXxy + EXYyy) * x;// Ex[c] * x * (x * y * Ex[c] + y * y * Ey[c]);
					
					t_A[9]  += EXX * yy; //Ex[c] * y * Ex[c] * y;
					t_A[10] += EXX * y;  //Ex[c] * y * Ex[c];
					t_A[11] += EXYxy; //Ex[c] * y * Ey[c] * x;
					t_A[12] += EXYyy; //Ex[c] * y * Ey[c] * y;
					t_A[13] += EXY * y;  //Ex[c] * y * Ey[c];
					t_A[14] += (EXXxx + EXYxy) * y;//Ex[c] * y * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[15] += (EXXxy + EXYyy) * y;//Ex[c] * y * (x * y * Ex[c] + y * y * Ey[c]);
					
					t_A[18] += EXX;		//Ex[c] * Ex[c];
					t_A[21] += EXY;		//Ex[c] * Ey[c];
					t_A[27] += EYY * xx;	//Ey[c] * x * Ey[c] * x;
					t_A[28] += EYYxy;	//Ey[c] * x * Ey[c] * y;
					t_A[29] += EYY * x;		//Ey[c] * x * Ey[c];
					t_A[30] += (EXYxx + EYYxy) * x;	//Ey[c] * x * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[31] += (EXYxy + EYYyy) * x;	//Ey[c] * x * (x * y * Ex[c] + y * y * Ey[c]);
					
					t_A[36] += EYYyy;	//Ey[c] * y * Ey[c] * y;
					t_A[37] += EYY * y;		//Ey[c] * y * Ey[c];
					t_A[38] += (EXYxx + EYYxy) * y;	//Ey[c] * y * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[39] += (EXYxy + EYYyy) * y;	//Ey[c] * y * (x * y * Ex[c] + y * y * Ey[c]);
					
					t_A[45] += EYY;			//Ey[c] * Ey[c];
					t_A[54] += (EXxxVsEYxy) * (EXxxVsEYxy);
					t_A[55] += (EXxxVsEYxy) * (EXxyVsEYyy);
					
					t_A[63] += (EXxyVsEYyy) * (EXxyVsEYyy);
					
					t_C[0] += -ExEt * x;
					t_C[1] += -ExEt * y;
					t_C[2] += -ExEt;
					t_C[3] += -EyEt * x;
					t_C[4] += -EyEt * y;
					t_C[5] += -EyEt;
					t_C[6] += -(EXxxVsEYxy) * Et[c];
					t_C[7] += -(EXxyVsEYyy) * Et[c];


					//t_A[8]  += EXXxy; //Ex[c] * y * Ex[c] * x;
					//t_A[16] += EXX * x;	//Ex[c] * Ex[c] * x;
					//t_A[17] += EXX * y;	//Ex[c] * Ex[c] * y;
					//t_A[19] += EXYx;	//Ex[c] * Ey[c] * x;
					//t_A[20] += EXYy;	//Ex[c] * Ey[c] * y;

					//t_A[22] += EXXxx + EXYxy;	//Ex[c] * (x * x * Ex[c] + x * y * Ey[c]);
					//t_A[23] += EXXxy + EXYyy;	//Ex[c] * (x * y * Ex[c] + y * y * Ey[c]);

					//t_A[24] += EXYxx;	//Ey[c] * x * Ex[c] * x;
					//t_A[25] += EXYxy;	//Ey[c] * x * Ex[c] * y;
					//t_A[26] += EXYx;		//Ey[c] * x * Ex[c];

					//t_A[32] += EXYxy;	//Ey[c] * y * Ex[c] * x;
					//t_A[33] += EXYyy;	//Ey[c] * y * Ex[c] * y;
					//t_A[34] += EXYy;		//Ey[c] * y * Ex[c];
					//t_A[35] += EYYxy;	//Ey[c] * y * Ey[c] * x;
					//t_A[40] += EXY * x;		//Ey[c] * Ex[c] * x;
					//t_A[41] += EXY * y;		//Ey[c] * Ex[c] * y;
					//t_A[42] += EXY;			//Ey[c] * Ex[c];
					//t_A[43] += EYY * x;		//Ey[c] * Ey[c] * x;
					//t_A[44] += EYY * y;		//Ey[c] * Ey[c] * y;
					//t_A[46] += EXYxx + EYYxy;	//Ey[c] * (x * x * Ex[c] + x * y * Ey[c]);
					//t_A[47] += EXYxy + EYYyy;	//Ey[c] * (x * y * Ex[c] + y * y * Ey[c]);

					//t_A[48] += (EXXxx + EXYxy) * x;	//(x * x * Ex[c] + x * y * Ey[c]) * Ex[c] * x;
					//t_A[49] += (EXXxx + EXYxy) * y;	//(x * x * Ex[c] + x * y * Ey[c]) * Ex[c] * y;
					//t_A[50] += EXXxx + EXYxy;			//(x * x * Ex[c] + x * y * Ey[c]) * Ex[c];
					//t_A[51] += (EXYxx + EYYxy) * x;	//(x * x * Ex[c] + x * y * Ey[c]) * Ey[c] * x;
					//t_A[52] += (EXYxx + EYYxy) * y;	//(x * x * Ex[c] + x * y * Ey[c]) * Ey[c] * y;
					//t_A[53] += EXYxx + EYYxy;			//(x * x * Ex[c] + x * y * Ey[c]) * Ey[c];

					//t_A[56] += (EXXxy + EXYyy) * x;		//(x * y * Ex[c] + y * y * Ey[c]) * Ex[c] * x;
					//t_A[57] += (EXXxy + EXYyy) * y;		//(x * y * Ex[c] + y * y * Ey[c]) * Ex[c] * y;
					//t_A[58] += EXXxy + EXYyy;				//(x * y * Ex[c] + y * y * Ey[c]) * Ex[c];
					//t_A[59] += x * (EXYxy + EYYyy);		//(x * y * Ex[c] + y * y * Ey[c]) * Ey[c] * x;
					//t_A[60] += y * (EXYxy + EYYyy);		//(x * y * Ex[c] + y * y * Ey[c]) * Ey[c] * y;
					//t_A[61] += EXYxy + EYYyy;				//(x * y * Ex[c] + y * y * Ey[c]) * Ey[c];
					//t_A[62] += (EXxy + EYyy) * (EXxx + EYxy);
				}
			}
			#pragma omp critical (crit_projection_firstiter)
			{
				for (int i = 0; i < 8; ++i)
					C[i] += t_C[i];
				for(int i = 0 ; i < 64; ++i)
					A[i] += t_A[i];
			}
		}
		A[19] = A[5];
		A[20] = A[13];
		A[23] = A[1] + A[12];
		A[22] = A[0] + A[4];
		A[46] = A[3] + A[28];
		A[47] = A[11] + A[36];
		for (int i = 1; i < 8; ++i)
		{
			for (int j = 0; j < i; ++j)
			{
				A[i * 8 + j] = A[j * 8 + i];
			}
		}
		fixTATC(A, C);
	} 
	else 
	{
		memset(C, 0, 8 * sizeof(double));
		#pragma omp parallel
		{
			double t_C[8] = { 0 };

			#pragma omp for schedule (/*dynamic, 16*/static)
			for (int j = shift; j < height - shift; ++j)
			{
				int jWidth = j * width;
				double y = j - height2;
				double yy = y * y;
				int c = shift + jWidth;
				int iMinWidth2 = shift - width2;
				for (int i = shift; i < width - shift; ++i, ++c, ++iMinWidth2) 
				{
					double x = iMinWidth2;
					double Etx = Ex[c] * Et[c];
					double Ety = Ey[c] * Et[c];
					double Etxx = Etx * x;
					double Etyy = Ety * y;
					double EXxEYy = Etxx  + Etyy;
					//t_
					t_C[0] -= Etxx;
					t_C[1] -= Etx * y;
					t_C[2] -= Etx;
					t_C[3] -= Ety * x;
					t_C[4] -= Etyy;
					t_C[5] -= Ety;
					t_C[6] -= EXxEYy * x;
					t_C[7] -= EXxEYy * y;
				}
			}

			#pragma omp critical (crit_projection)
			{
				for (int i = 0; i < 8; ++i)
				{
					C[i] += t_C[i];
				}
			}
		}
		for (int i = 0; i < 8; ++i)
		{
			C[i] /= 2;
		}
	}
	return 1;
}

void Projection::fixTATC(double* t_A, double *t_C) const
{
	for (int i = 0; i < 64; ++i)
	{
		t_A[i] /= 4;
	}
	for (int i = 0; i < 8; ++i)
	{
		t_C[i] /= 2;
	}
	
}


int Projection::prepare_proj_picture()
{
	if(!proj_picture || !width || !height) return 0;

	converter.applyProjGrey(im1, width, height, proj, proj_picture);
	
	return 1;
}

void Projection::estimate_q_fromAC(double const* A0, double const* C0)
{
	for (int i = 0; i < 8; ++i)
	{
		q[i] = 0;
		for (int j = 0; j < 8; ++j)
		{
			q[i] += A0[i * 8 + j] * C0[j];
		}
	}
}


int Projection::calculate_q()
{

	double A0[64];
	memcpy(A0, A, 64 * sizeof(double));
	double w[8], u[64], v[64], buffer[3*8];
	SVD(A0, 8, 8, 8, w, u, 8, v, 8, buffer);
	SVBkSb(8, 8, w, u, 8, v, 8, 0, 0, 0, A0, 8, buffer);
	
	estimate_q_fromAC(A0, C);

	double x1 = -width / 2.0 , y1 = -height / 2.0, 
			x2 = -width / 2.0, y2 = height / 2.0, 
			x3 = width / 2.0, y3 = -height / 2.0, 
			x4 = width / 2.0, y4 = height / 2.0;

	double x1s =  x1 + q[0] * x1 + q[1] * y1 + q[2] + q[6] * x1 * x1 + q[7] * x1 * y1;
	double y1s =  y1 + q[3] * x1 + q[4] * y1 + q[5] + q[6] * x1 * y1 + q[7] * y1 * y1;

	double x2s =  x2 + q[0] * x2 + q[1] * y2 + q[2] + q[6] * x2 * x2 + q[7] * x2 * y2;
	double y2s =  y2 + q[3] * x2 + q[4] * y2 + q[5] + q[6] * x2 * y2 + q[7] * y2 * y2;

	double x3s =  x3 + q[0] * x3 + q[1] * y3 + q[2] + q[6] * x3 * x3 + q[7] * x3 * y3;
	double y3s =  y3 + q[3] * x3 + q[4] * y3 + q[5] + q[6] * x3 * y3 + q[7] * y3 * y3;

	double x4s =  x4 + q[0] * x4 + q[1] * y4 + q[2] + q[6] * x4 * x4 + q[7] * x4 * y4;
	double y4s =  y4 + q[3] * x4 + q[4] * y4 + q[5] + q[6] * x4 * y4 + q[7] * y4 * y4;
	

	A0[0] = x1;  A0[1] = y1;  A0[2] = 1;  A0[3] = 0;  A0[4] = 0;  A0[5] = 0;  A0[6] = -x1*x1s;  A0[7] = -y1*x1s; 
	A0[8] =  0;  A0[9] = 0;   A0[10] = 0; A0[11] = x1;A0[12] = y1;A0[13] = 1; A0[14] = -x1*y1s; A0[15] = -y1*y1s; 
	A0[16] = x2; A0[17] = y2; A0[18] = 1; A0[19] = 0; A0[20] = 0; A0[21] = 0; A0[22] = -x2*x2s; A0[23] = -y2*x2s; 
	A0[24] =  0; A0[25] = 0;  A0[26] = 0; A0[27] = x2;A0[28] = y2;A0[29] = 1; A0[30] = -x2*y2s; A0[31] = -y2*y2s; 
	A0[32] = x3; A0[33] = y3; A0[34] = 1; A0[35] = 0; A0[36] = 0; A0[37] = 0; A0[38] = -x3*x3s; A0[39] = -y3*x3s; 
	A0[40] =  0; A0[41] = 0;  A0[42] = 0; A0[43] = x3;A0[44] = y3;A0[45] = 1; A0[46] = -x3*y3s; A0[47] = -y3*y3s; 
	A0[48] = x4; A0[49] = y4; A0[50] = 1; A0[51] = 0; A0[52] = 0; A0[53] = 0; A0[54] = -x4*x4s; A0[55] = -y4*x4s; 
	A0[56] =  0; A0[57] = 0;  A0[58] = 0; A0[59] = x4;A0[60] = y4;A0[61] = 1; A0[62] = -x4*y4s; A0[63] = -y4*y4s; 

	double C0[8];
	C0[0] = x1s; C0[1] = y1s; C0[2] = x2s; C0[3] = y2s; C0[4] = x3s; C0[5] = y3s;
	C0[6] = x4s; C0[7] = y4s;
	
	SVD(A0, 8, 8, 8, w, u, 8, v, 8, buffer);
	SVBkSb(8, 8, w, u, 8, v, 8, 0, 0, 0, A0, 8, buffer);

	estimate_q_fromAC(A0, C0);
	return 1;
}

int Projection::calculate_proj()
{
	if(!proj || !q) return 0;
		
	double proj_new[8];
	double denom = proj[6] * q[2] + proj[7] * q[5] + 1;
	
	if(!denom) return 0;
	
	proj_new[0] = proj[0] * q[0] + proj[1] * q[3] + proj[2] * q[6];
	proj_new[1] = proj[0] * q[1] + proj[1] * q[4] + proj[2] * q[7];
	proj_new[2] = proj[0] * q[2] + proj[1] * q[5] + proj[2];
	proj_new[3] = proj[3] * q[0] + proj[4] * q[3] + proj[5] * q[6];
	proj_new[4] = proj[3] * q[1] + proj[4] * q[4] + proj[5] * q[7];
	proj_new[5] = proj[3] * q[2] + proj[4] * q[5] + proj[5];
	proj_new[6] = proj[6] * q[0] + proj[7] * q[3] + q[6];
	proj_new[7] = proj[6] * q[1] + proj[7] * q[4] + q[7];
	
	proj_new[0] /= denom;
	proj_new[1] /= denom;
	proj_new[2] /= denom;
	proj_new[3] /= denom;
	proj_new[4] /= denom;
	proj_new[5] /= denom;
	proj_new[6] /= denom;
	proj_new[7] /= denom;

	memcpy(proj, proj_new, 8 * sizeof(double));

	return 1;
}

int Projection::clear_data()
{
	initProj(proj);

	if(proj_picture)	{delete[] proj_picture; proj_picture = NULL;}
	if(Ex)				{delete[] Ex; Ex = NULL;}
	if(Ey)				{delete[] Ey; Ey = NULL;}
	if(Et)				{delete[] Et; Et = NULL;}
	
	width = 0;
	height = 0;

	memset(A, 0, 64 * sizeof(double));
	memset(C, 0, 8 * sizeof(double));
	memcpy(q, proj, 8 * sizeof(double));

	return 1;
}
unsigned char* Projection::take_picture_projection()
{
	return proj_picture;
}
unsigned char* Projection::take_picture_two()
{
	return im2;
}
int Projection::reset()
{
	clear_data();
	return 1;
}
int Projection::change_iterNum(int new_iterNum)
{
	if(new_iterNum < 1)
		return 0;
	iterNum = new_iterNum;
	return 1;
}
int Projection::multiply_projection(double* lp, double* rp, double* result)
{
	double proj_new[8];
	double denom = lp[6] * rp[2] + lp[7] * rp[5] + 1;
	if(!denom)
		return 0;
	proj_new[0] = lp[0] * rp[0] + lp[1] * rp[3] + lp[2] * rp[6];
	proj_new[1] = lp[0] * rp[1] + lp[1] * rp[4] + lp[2] * rp[7];
	proj_new[2] = lp[0] * rp[2] + lp[1] * rp[5] + lp[2];
	proj_new[3] = lp[3] * rp[0] + lp[4] * rp[3] + lp[5] * rp[6];
	proj_new[4] = lp[3] * rp[1] + lp[4] * rp[4] + lp[5] * rp[7];
	proj_new[5] = lp[3] * rp[2] + lp[4] * rp[5] + lp[5];
	proj_new[6] = lp[6] * rp[0] + lp[7] * rp[3] + rp[6];
	proj_new[7] = lp[6] * rp[1] + lp[7] * rp[4] + rp[7];

	proj_new[0] /= denom;
	proj_new[1] /= denom;
	proj_new[2] /= denom;
	proj_new[3] /= denom;
	proj_new[4] /= denom;
	proj_new[5] /= denom;
	proj_new[6] /= denom;
	proj_new[7] /= denom;

	memcpy(result, proj_new, 8 * sizeof(double));
	return 1;
}
int Projection::change_gaussDepth(int new_gaussDepth)
{
	if(new_gaussDepth < 0)
		return 0;
	gauss_depth = new_gaussDepth;
	return 1;
}
int Projection::mat3x3_invert(double *m, double *m_inv)
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





