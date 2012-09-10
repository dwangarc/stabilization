#include "projection.h"
#include <math.h>
//#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <omp.h>
//#include <assert.h>
//#include <float.h>

//SVD to solve the equation (include svd.cpp)
void	SVD(double* a, int lda, int m, int n, double* w, double* uT, 
			int lduT, double* vT, int ldvT, double* buffer);
void	SVBkSb(int m, int n, double* w, double* uT, int lduT, double* vT, 
			int ldvT, double* b, int ldb, int nb, double* x, int ldx, double* buffer);	

Projection::Projection() 
{
	proj[0] = 1.0; proj[1] = 0.0; proj[2] = 0.0;
	proj[3] = 0.0; proj[4] = 1.0; proj[5] = 0.0;
	proj[6] = 0.0; proj[7] = 0.0;
	
	stab_proj[0] = 1.0; stab_proj[1] = 0.0; stab_proj[2] = 0.0;
	stab_proj[3] = 0.0; stab_proj[4] = 1.0; stab_proj[5] = 0.0;
	stab_proj[6] = 0.0; stab_proj[7] = 0.0;

	/*for(int i = 0; i < 11; i++){
		st[i] = new double[8];
		st[i][0] = 1.0; st[i][1] = 0.0; st[i][2] = 0.0;
		st[i][3] = 0.0; st[i][4] = 1.0; st[i][5] = 0.0;
		st[i][6] = 0.0; st[i][7] = 0.0;	
	}*/


	im1 = NULL;
	im2 = NULL;
	//mask_one = NULL;
	//mask_two = NULL;

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
	if(im1)				{delete[] im1; im1 = NULL;}
	if(im2)				{delete[] im2; im2 = NULL;}
	if(Ex)				{delete[] Ex; Ex = NULL;}
	if(Ey)				{delete[] Ey; Ey = NULL;}
	if(Et)				{delete[] Et; Et = NULL;}
	if(proj_picture)	{delete[] proj_picture; proj_picture = NULL;}
//	if(stab_picture)	{delete[] stab_picture; stab_picture = NULL;}
	//if(mask_one)		{delete[] mask_one; mask_one = NULL;}
	//if(mask_two)		{delete[] mask_two; mask_two = NULL;}
	return;
}
int Projection::load_im(unsigned char* im, int im_width, int im_height)
{
	if(!im || !im_width || !im_height)
		return 0;
	if(!width || !height) {width = im_width; height = im_height;}
	if(width != im_width || height != im_height)
		return 0;
	if(!im2){
		clear_data();
		width = im_width;
		height = im_height;
		im2 = new unsigned char[width * height];
		im1 = new unsigned char[width * height];
		memcpy(im2, im, width * height * sizeof(unsigned char));
		for(int i = 0; i < gauss_depth; i++)
			gauss_filter(im2, width, height);
		
		//mask_one = new unsigned char[width * height];
		//memset(mask_one, 0, width * height * sizeof(unsigned char));
		//mask_two = new unsigned char[width * height];
		//memset(mask_two, 0, width * height * sizeof(unsigned char));
		proj_picture = new unsigned char[width * height];
		memset(proj_picture, 0, width * height * sizeof(unsigned char));
		Ex = new double[width * height];
		Ey = new double[width * height];
		Et = new double[width * height];
	}
	unsigned char* temp = im1;
	im1 = im2;
	im2 = temp;
	memcpy(im2, im, width * height * sizeof(unsigned char));
	for(int i = 0; i < gauss_depth; i++)
	{
		gauss_filter(im2, width, height);
	}

	proj[0] = 1.0; proj[1] = 0.0; proj[2] = 0.0;
	proj[3] = 0.0; proj[4] = 1.0; proj[5] = 0.0;
	proj[6] = 0.0; proj[7] = 0.0;
	
	for (int iter = 1; iter <= iterNum; ++iter) 
	{
		calculate_maps(iter);
		calculate_matrix(iter);

		calculate_q();
		calculate_proj();

		prepare_proj_picture();
	}
	return 1;
}


int Projection::gauss_filter(unsigned char* im, int im_width, int im_height) const
{
	if(!im || !im_width || !im_height)
		return 0;
	unsigned char* dxx[5];
	int* gxx = new int[im_width];
	int* Gxx = new int[im_width - 4];
	int* s = new int[(im_width - 4) * (im_height - 4)];

	dxx[1] = im; dxx[2] = im + im_width;
	dxx[3] = im + 2 * im_width; dxx[4] = im + 3 * im_width;

	for(int j = 2; j < im_height - 2; j++){
		dxx[0] = dxx[1]; dxx[1] = dxx[2]; 
		dxx[2] = dxx[3]; dxx[3] = dxx[4];
		dxx[4] += im_width;

		gauss_sweep_one(dxx, im_width, gxx);
		gauss_sweep_two(gxx, im_width, Gxx);
		gauss_sweep_three(Gxx, im_width, s + (j - 2) * (im_width - 4));
	}
	gauss_sweep_four(s, im_width, im_height, im);

	delete [] gxx;
	delete [] Gxx;
	delete [] s;

	return 1;
}
int Projection::gauss_sweep_one(unsigned char** dxx, int im_width, int* gxx) const
{
	for(int i = 0; i < im_width; i++)
		gxx[i] = dxx[0][i] + (dxx[1][i] << 2) + (dxx[2][i] << 2) + (dxx[2][i] << 1) +
				(dxx[3][i] << 2) + dxx[4][i];
	return 1;	
}
int Projection::gauss_sweep_two(int* gxx, int im_width, int* Gxx) const
{
	for(int i = 0; i < im_width - 4; i++)
		Gxx[i] = gxx[i] + (gxx[i + 1] << 2) + (gxx[i + 2] << 2) + 
			(gxx[i + 2] << 1) + (gxx[i + 3] << 2) + gxx[i + 4];
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
	for(int j = 2; j < im_height - 2; j++)
		for(int i = 2; i < im_width - 2; i++)
			im[i + j * im_width] = s[i - 2 + (j - 2) * (im_width - 4)];
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
	if(iteration == 1) {
		memset(Ex, 0, width * height * sizeof(double));
		memset(Ey, 0, width * height * sizeof(double));  
		memset(Et, 0, width * height * sizeof(double));
		#pragma omp parallel
		{
			#pragma omp for
			for (int j = bandWidth; j < height - bandWidth; j++)
				for (int i = bandWidth; i < width - bandWidth; i++) {
					Et[i + j * width] = (im1[i + j * width] - im2[i + j * width]);
					Ex[i + j * width] = /*calculateDerX(i, j);*/(im2[i + 1 + j * width] - im2[i - 1 + j * width]) / 2.0;
					Ey[i + j * width] = /*calculateDerY(i, j);*/(im2[i + (j + 1) * width] - im2[i + (j - 1) * width]) / 2.0;
		}
	}
	} else {
		memset(Et, 0, width * height * sizeof(double));
		#pragma omp parallel
		{
			#pragma omp for	
			for (int j = bandWidth; j < height - bandWidth; j++)
				for (int i = bandWidth; i < width - bandWidth; i++){
					if(!proj_picture[i + j * width]){
						Et[i + j * width] = 0;
						continue;
					}
					Et[i + j * width] = (proj_picture[i + j * width] - im2[i + j * width]);
				}
		}
	}

	return 1;
}
int Projection::calculate_matrix(int iteration)
{
	if(!Ex || !Ey || !Et || /*!mask_one || !mask_two ||*/ !width || !height) 
		return 0;

	int shift = width / 20;
	if(iteration == 1){
		memset(A, 0, 64 * sizeof(double));
		memset(C, 0, 8 * sizeof(double));
		#pragma omp parallel
		{
			double t_C[8] = { 0 };
			double t_A[64] = { 0 };

			#pragma omp for schedule (dynamic, 64)
			for (int j = shift; j < height - shift; j++)
				for (int i = shift; i < width - shift; i++) {
					int c = i + j * width;
/*					if(mask_one[c] > 0 || mask_two[c] > 0)
						continue;*/

					double x = i - width / 2.0;
					double y = j - height / 2.0;

					double xx = x * x;
					double yy = y * y;
					double xy = x * y;
					double EXX = Ex[c] * Ex[c];
					double EYY = Ey[c] * Ey[c];
					double EXY = Ex[c] * Ey[c];

					double EXXxx = EXX * xx;
					double EXXxy = EXX * xy;
					double EXXyy = EXX * yy;
					double EXYxx = EXY * xx;
					double EXYxy = EXY * xy;
					double EXYyy = EXY * yy;
					double EYYxx = EYY * xx;
					double EYYxy = EYY * xy;
					double EYYyy = EYY * yy;


					t_A[0] += EXXxx;//Ex[c] * x * Ex[c] * x;
					t_A[1] += EXXxy;//Ex[c] * x * Ex[c] * y;
					t_A[2] += EXX * x; //Ex[c] * x * Ex[c];
					t_A[3] += EXYxx;//Ex[c] * x * Ey[c] * x;
					t_A[4] += EXYxy;//Ex[c] * x * Ey[c] * y;
					t_A[5] += EXY * x; //Ex[c] * x * Ey[c];
					t_A[6] += x * (EXXxx + EXYxy);// Ex[c] * x * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[7] += x * (EXXxy + EXYyy);// Ex[c] * x * (x * y * Ex[c] + y * y * Ey[c]);
					
					t_A[8]  += EXXxy; //Ex[c] * y * Ex[c] * x;
					t_A[9]  += EXXyy; //Ex[c] * y * Ex[c] * y;
					t_A[10] += EXX * y;  //Ex[c] * y * Ex[c];
					t_A[11] += EXYxy; //Ex[c] * y * Ey[c] * x;
					t_A[12] += EXYyy; //Ex[c] * y * Ey[c] * y;
					t_A[13] += EXY * y;  //Ex[c] * y * Ey[c];
					t_A[14] += y * (EXXxx + EXYxy);//Ex[c] * y * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[15] += y * (EXXxy + EXYyy);//Ex[c] * y * (x * y * Ex[c] + y * y * Ey[c]);
					
					t_A[16] += EXX * x;	//Ex[c] * Ex[c] * x;
					t_A[17] += EXX * y;	//Ex[c] * Ex[c] * y;
					t_A[18] += EXX;		//Ex[c] * Ex[c];
					t_A[19] += EXY * x;	//Ex[c] * Ey[c] * x;
					t_A[20] += EXY * y;	//Ex[c] * Ey[c] * y;
					t_A[21] += EXY;		//Ex[c] * Ey[c];
					t_A[22] += EXXxx + EXYxy;	//Ex[c] * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[23] += EXXxy + EXYyy;	//Ex[c] * (x * y * Ex[c] + y * y * Ey[c]);
					
					t_A[24] += EXYxx;	//Ey[c] * x * Ex[c] * x;
					t_A[25] += EXYxy;	//Ey[c] * x * Ex[c] * y;
					t_A[26] += EXY * x;		//Ey[c] * x * Ex[c];
					t_A[27] += EYYxx;	//Ey[c] * x * Ey[c] * x;
					t_A[28] += EYYxy;	//Ey[c] * x * Ey[c] * y;
					t_A[29] += EYY * x;		//Ey[c] * x * Ey[c];
					t_A[30] += x * (EXYxx + EYYxy);	//Ey[c] * x * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[31] += x * (EXYxy + EYYyy);	//Ey[c] * x * (x * y * Ex[c] + y * y * Ey[c]);
					
					t_A[32] += EXYxy;	//Ey[c] * y * Ex[c] * x;
					t_A[33] += EXYyy;	//Ey[c] * y * Ex[c] * y;
					t_A[34] += EXY * y;		//Ey[c] * y * Ex[c];
					t_A[35] += EYYxy;	//Ey[c] * y * Ey[c] * x;
					t_A[36] += EYYyy;	//Ey[c] * y * Ey[c] * y;
					t_A[37] += EYY * y;		//Ey[c] * y * Ey[c];
					t_A[38] += y * (EXYxx + EYYxy);	//Ey[c] * y * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[39] += y * (EXYxy + EYYyy);	//Ey[c] * y * (x * y * Ex[c] + y * y * Ey[c]);
					
					t_A[40] += EXY * x;		//Ey[c] * Ex[c] * x;
					t_A[41] += EXY * y;		//Ey[c] * Ex[c] * y;
					t_A[42] += EXY;			//Ey[c] * Ex[c];
					t_A[43] += EYY * x;		//Ey[c] * Ey[c] * x;
					t_A[44] += EYY * y;		//Ey[c] * Ey[c] * y;
					t_A[45] += EYY;			//Ey[c] * Ey[c];
					t_A[46] += EXYxx + EYYxy;	//Ey[c] * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[47] += EXYxy + EYYyy;	//Ey[c] * (x * y * Ex[c] + y * y * Ey[c]);
					
					t_A[48] += x * (EXXxx + EXYxy);	//(x * x * Ex[c] + x * y * Ey[c]) * Ex[c] * x;
					t_A[49] += y * (EXXxx + EXYxy);	//(x * x * Ex[c] + x * y * Ey[c]) * Ex[c] * y;
					t_A[50] += EXXxx + EXYxy;			//(x * x * Ex[c] + x * y * Ey[c]) * Ex[c];
					t_A[51] += x * (EXYxx + EYYxy);	//(x * x * Ex[c] + x * y * Ey[c]) * Ey[c] * x;
					t_A[52] += y * (EXYxx + EYYxy);	//(x * x * Ex[c] + x * y * Ey[c]) * Ey[c] * y;
					t_A[53] += EXYxx + EYYxy;			//(x * x * Ex[c] + x * y * Ey[c]) * Ey[c];
					t_A[54] += (xx * Ex[c] + xy * Ey[c]) * (xx * Ex[c] + xy * Ey[c]);
					t_A[55] += (xx * Ex[c] + xy * Ey[c]) * (xy * Ex[c] + yy * Ey[c]);
					
					t_A[56] += x * (EXXxy + EXYyy);		//(x * y * Ex[c] + y * y * Ey[c]) * Ex[c] * x;
					t_A[57] += y * (EXXxy + EXYyy);		//(x * y * Ex[c] + y * y * Ey[c]) * Ex[c] * y;
					t_A[58] += EXXxy + EXYyy;				//(x * y * Ex[c] + y * y * Ey[c]) * Ex[c];
					t_A[59] += x * (EXYxy + EYYyy);		//(x * y * Ex[c] + y * y * Ey[c]) * Ey[c] * x;
					t_A[60] += y * (EXYxy + EYYyy);		//(x * y * Ex[c] + y * y * Ey[c]) * Ey[c] * y;
					t_A[61] += EXYxy + EYYyy;				//(x * y * Ex[c] + y * y * Ey[c]) * Ey[c];
					t_A[62] += (xy * Ex[c] + yy * Ey[c]) * (xx * Ex[c] + xy * Ey[c]);
					t_A[63] += (xy * Ex[c] + yy * Ey[c]) * (xy * Ex[c] + yy * Ey[c]);
					
					t_C[0] += -Et[c] * Ex[c] * x;
					t_C[1] += -Et[c] * Ex[c] * y;
					t_C[2] += -Et[c] * Ex[c];
					t_C[3] += -Et[c] * Ey[c] * x;
					t_C[4] += -Et[c] * Ey[c] * y;
					t_C[5] += -Et[c] * Ey[c];
					t_C[6] += -Et[c] * (xx * Ex[c] + xy * Ey[c]);
					t_C[7] += -Et[c] * (xy * Ex[c] + yy * Ey[c]);
			
			}
			#pragma omp critical (crit_projection_firstiter)
			{
				for (int i = 0; i < 8; i++)
					C[i] += t_C[i];
				for(int i = 0 ; i < 64; i++)
					A[i] += t_A[i];
			}
		}
	} else {
		memset(C, 0, 8 * sizeof(double));
		#pragma omp parallel
		{
			double t_C[8] = { 0 };

			#pragma omp for schedule (dynamic, 64)
			for (int j = shift; j < height - shift; j++)
				for (int i = shift; i < width - shift; i++) {
					int c = i + j * width;
/*					if(mask_one[c] > 0 || mask_two[c] > 0)
						continue;*/

					double x = i - width  * 0.5;
					double y = j - height * 0.5;
					
					t_C[0] += -Et[c] * Ex[c] * x;
					t_C[1] += -Et[c] * Ex[c] * y;
					t_C[2] += -Et[c] * Ex[c];
					t_C[3] += -Et[c] * Ey[c] * x;
					t_C[4] += -Et[c] * Ey[c] * y;
					t_C[5] += -Et[c] * Ey[c];
					t_C[6] += -Et[c] * (x * x * Ex[c] + x * y * Ey[c]);
					t_C[7] += -Et[c] * (x * y * Ex[c] + y * y * Ey[c]);
				}

			#pragma omp critical (crit_projection)
			{
				for (int i = 0; i < 8; i++)
					C[i] += t_C[i];
			}
		}
	}
	return 1;
}
int Projection::prepare_proj_picture()
{
	if(!proj_picture || !width || !height) return 0;
	
	memset(proj_picture, 0, width * height * sizeof(unsigned char));

	#pragma omp parallel
	{
		#pragma omp for
		for(int j = 0; j < height; j++)
			for(int i = 0; i < width; i++){
				double ip = i - width * 0.5;
				double jp = j - height * 0.5;
				double norm = 1 / (proj[6] * ip + proj[7] * jp + 1);
				double x = (proj[0] * ip + proj[1] * jp + proj[2]) * norm;
				double y = (proj[3] * ip + proj[4] * jp + proj[5]) * norm;

				x += width  * 0.5;
				y += height * 0.5;
				if (x < 0 || x > (width - 1) || y < 0 || y > (height - 1)) {
					proj_picture[i + j * width] = 0;
					continue;
				}
				int ix = int(x);
				int iy = int(y);
				double dx = x - ix;
				double dy = y - iy;
				proj_picture[i + j * width] = 
					unsigned char((1 - dx) * (1 - dy) * im1[ix + iy * width] + 
					(1 - dx) * dy * im1[ix + (iy + 1) * width] + 
					dx * (1 - dy) * im1[ix + 1 + iy * width] + 
					dx * dy * im1[ix + 1 + (iy + 1) * width] + 0.5);
			}
	}
		return 1;
}
int Projection::calculate_q()
{

	double A0[64];
	memcpy(A0, A, 64 * sizeof(double));
	double w[8], u[64], v[64], buffer[3*8];
	SVD(A0, 8, 8, 8, w, u, 8, v, 8, buffer);
	SVBkSb(8, 8, w, u, 8, v, 8, 0, 0, 0, A0, 8, buffer);
	
	q[0] = A0[0] * C[0] + A0[1] * C[1] + A0[2] * C[2] + A0[3] * C[3] +
			A0[4] * C[4] + A0[5] * C[5] + A0[6] * C[6] + A0[7] * C[7];
	q[1] = A0[8] * C[0] + A0[9] * C[1] + A0[10] * C[2] + A0[11] * C[3] +
		A0[12] * C[4] + A0[13] * C[5] + A0[14] * C[6] + A0[15] * C[7];
	q[2] = A0[16] * C[0] + A0[17] * C[1] + A0[18] * C[2] + A0[19] * C[3] +
		A0[20] * C[4] + A0[21] * C[5] + A0[22] * C[6] + A0[23] * C[7];
	q[3] = A0[24] * C[0] + A0[25] * C[1] + A0[26] * C[2] + A0[27] * C[3] +
		A0[28] * C[4] + A0[29] * C[5] + A0[30] * C[6] + A0[31] * C[7];

	q[4] = A0[32] * C[0] + A0[33] * C[1] + A0[34] * C[2] + A0[35] * C[3] +
		A0[36] * C[4] + A0[37] * C[5] + A0[38] * C[6] + A0[39] * C[7];
	q[5] = A0[40] * C[0] + A0[41] * C[1] + A0[42] * C[2] + A0[43] * C[3] +
		A0[44] * C[4] + A0[45] * C[5] + A0[46] * C[6] + A0[47] * C[7];
	q[6] = A0[48] * C[0] + A0[49] * C[1] + A0[50] * C[2] + A0[51] * C[3] +
		A0[52] * C[4] + A0[53] * C[5] + A0[54] * C[6] + A0[55] * C[7];
	q[7] = A0[56] * C[0] + A0[57] * C[1] + A0[58] * C[2] + A0[59] * C[3] +
		A0[60] * C[4] + A0[61] * C[5] + A0[62] * C[6] + A0[63] * C[7];

	double x1 = -width / 2.0 , y1 = -height / 2.0, 
			x2 = -width / 2.0, y2 = height / 2.0, 
			x3 = width / 2.0, y3 = -height / 2.0, 
			x4 = width / 2.0, y4 = height / 2.0;

	/*double x1 = 0, y1 = 0,
		x2 = 0, y2 = height,
		x3 = width, y3 = 0,
		x4 = width, y4 = height;*/
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

	q[0] = A0[0] * C0[0] + A0[1] * C0[1] + A0[2] * C0[2] + A0[3] * C0[3] +
		A0[4] * C0[4] + A0[5] * C0[5] + A0[6] * C0[6] + A0[7] * C0[7];
	q[1] = A0[8] * C0[0] + A0[9] * C0[1] + A0[10] * C0[2] + A0[11] * C0[3] +
		A0[12] * C0[4] + A0[13] * C0[5] + A0[14] * C0[6] + A0[15] * C0[7];
	q[2] = A0[16] * C0[0] + A0[17] * C0[1] + A0[18] * C0[2] + A0[19] * C0[3] +
		A0[20] * C0[4] + A0[21] * C0[5] + A0[22] * C0[6] + A0[23] * C0[7];
	q[3] = A0[24] * C0[0] + A0[25] * C0[1] + A0[26] * C0[2] + A0[27] * C0[3] +
		A0[28] * C0[4] + A0[29] * C0[5] + A0[30] * C0[6] + A0[31] * C0[7];

	q[4] = A0[32] * C0[0] + A0[33] * C0[1] + A0[34] * C0[2] + A0[35] * C0[3] +
		A0[36] * C0[4] + A0[37] * C0[5] + A0[38] * C0[6] + A0[39] * C0[7];
	q[5] = A0[40] * C0[0] + A0[41] * C0[1] + A0[42] * C0[2] + A0[43] * C0[3] +
		A0[44] * C0[4] + A0[45] * C0[5] + A0[46] * C0[6] + A0[47] * C0[7];
	q[6] = A0[48] * C0[0] + A0[49] * C0[1] + A0[50] * C0[2] + A0[51] * C0[3] +
		A0[52] * C0[4] + A0[53] * C0[5] + A0[54] * C0[6] + A0[55] * C0[7];
	q[7] = A0[56] * C0[0] + A0[57] * C0[1] + A0[58] * C0[2] + A0[59] * C0[3] +
		A0[60] * C0[4] + A0[61] * C0[5] + A0[62] * C0[6] + A0[63] * C0[7];
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
//int Projection::load_mask_one(unsigned char* mask)
//{
//	if(!mask || !width || !height || !mask_one)
//		return 0;
//	memcpy(mask_one, mask, width * height * sizeof(unsigned char));
//	return 1;
//}
//int Projection::load_mask_two(unsigned char* mask)
//{
//	if(!mask || !width || !height || !mask_two)
//		return 0;
//	memcpy(mask_two, mask, width * height * sizeof(unsigned char));
//	return 1;
//}
int Projection::clear_data()
{
	proj[0] = 1.0; proj[1] = 0.0; proj[2] = 0.0;
	proj[3] = 0.0; proj[4] = 1.0; proj[5] = 0.0;
	proj[6] = 0.0; proj[7] = 0.0;

	if(im1)				{delete[] im1; im1 = NULL;}
	if(im2)				{delete[] im2; im2 = NULL;}
	//if(mask_one)		{delete[] mask_one; mask_one = NULL;}
	//if(mask_two)		{delete[] mask_two; mask_two = NULL;}
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





