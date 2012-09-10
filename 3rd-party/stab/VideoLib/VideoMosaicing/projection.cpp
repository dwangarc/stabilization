#include "projection.h"
#include <new>
#include <iostream>
#include <math.h>
#include <omp.h>
//#include <Utils/ppm.h>

using std::cout;
using std::endl;
using std::bad_alloc;

#define BASIC_SIZE		320
#define MAX_SIZE		BASIC_SIZE * 6
#define MIN_SIZE		BASIC_SIZE / 2
#define DIV_MASK_BORDER	2

void	SVD(double* a, int lda, int m, int n, double* w, double* uT,
			int lduT, double* vT, int ldvT, double* buffer);
void	SVBkSb(int m, int n, double* w, double* uT, int lduT, double* vT,
			int ldvT, double* b, int ldb, int nb, double* x, int ldx, double* buffer);

Projection::Projection()
{
	proj[0] = 1.0; proj[1] = 0.0; proj[2] = 0.0;
	proj[3] = 0.0; proj[4] = 1.0; proj[5] = 0.0;
	proj[6] = 0.0; proj[7] = 0.0;

	im1 = NULL;
	im2 = NULL;
	proj_picture = NULL;

	width = 0;
	height = 0;

	mask_one = NULL;
	mask_two = NULL;
	mask_div = NULL;

	gauss_depth = 1;
	iterNum = 10;

	Ex = NULL;
	Ey = NULL;
	Et = NULL;
	
	memset(A, 0, 64 * sizeof(double));
	memset(C, 0, 8 * sizeof(double));
	memcpy(q, proj, 8 * sizeof(double));
}
Projection::~Projection()
{
	if(im1)				{delete[] im1; im1 = NULL;}
	if(im2)				{delete[] im2; im2 = NULL;}
	if(proj_picture)	{delete[] proj_picture; proj_picture = NULL;}
	if(mask_one)		{delete[] mask_one; mask_one = NULL;}
	if(mask_two)		{delete[] mask_two; mask_two = NULL;}
	if(mask_div)		{delete[] mask_div; mask_div = NULL;}
	if(Ex)				{delete[] Ex; Ex = NULL;}
	if(Ey)				{delete[] Ey; Ey = NULL;}
	if(Et)				{delete[] Et; Et = NULL;}
	return;
}
int Projection::load_im(unsigned char* im, int im_width, int im_height)
{
	try{
		if(!im || !im_width || !im_height)
			throw "False image, width or height.";
		
		if(im2){
			if(!im1 || !mask_one || !mask_two || !proj_picture || !Ex || !Ey || !Et || !width || !height){
				clear_data();
				throw "Load block isn't ready.";
			}
			if(width != im_width || height != im_height)
				throw "Different image size.";
		} else{
			if(im1 || mask_one || mask_two || proj_picture || Ex || Ey || Et || width || height
				|| im_width > MAX_SIZE || im_height > MAX_SIZE || im_width < MIN_SIZE || im_height < MIN_SIZE){
				clear_data();
				throw "Load block is working improperly.";
			}
			width = im_width;
			height = im_height;
			
			im2 = new unsigned char[width * height];
			memcpy(im2, im, width * height * sizeof(unsigned char));
			for(int i = 0; i < gauss_depth; i++)
				gauss_filter(im2, width, height);

			im1 = new unsigned char[width * height];
			memcpy(im1, im2, width * height * sizeof(unsigned char));

			mask_one = new unsigned char[width * height];
			memset(mask_one, 0, width * height * sizeof(unsigned char));
			
			mask_two = new unsigned char[width * height];
			memset(mask_two, 0, width * height * sizeof(unsigned char));

			proj_picture = new unsigned char[width * height];
			memset(proj_picture, 0, width * height * sizeof(unsigned char));
			
			Ex = new double[width * height];
			memset(Ex, 0, width * height * sizeof(double));
			
			Ey = new double[width * height];
			memset(Ey, 0, width * height * sizeof(double));

			Et = new double[width * height];
			memset(Et, 0, width * height * sizeof(double));

			return 1;
		}
		
		unsigned char* temp = im1;
		im1 = im2;
		im2 = temp;
		
		memcpy(im2, im, width * height * sizeof(unsigned char));
		for(int i = 0; i < gauss_depth; i++)
			gauss_filter(im2, width, height);
	} catch(const char message[]){
		cout << message << endl;
		return 0;
	} catch(bad_alloc &ex){
		cout << "Failed to allocate memory." << endl;
		return 0;
	} catch(...){
		cout << "Load block's failed.";
		return 0;
	}

	int pyr_num = static_cast<int>(sqrt(width / static_cast<double>(BASIC_SIZE)) + 3.5);

	int* pyr_width;
	pyr_width = new int[pyr_num];
	int* pyr_height;
	pyr_height = new int[pyr_num];

	pyr_width[pyr_num - 1] = width;
	pyr_height[pyr_num - 1] = height;
	
	for(int i = pyr_num - 2; i >= 0; i--){
		pyr_width[i] = pyr_width[i + 1] / 2;
		pyr_height[i] = pyr_height[i + 1] / 2;
	}
	unsigned char** pyr_one;
	unsigned char** pyr_two;
	unsigned char** mask_pyr_one;
	unsigned char** mask_pyr_two;
	
	try{
		pyr_one = new unsigned char*[pyr_num];
		pyr_two = new unsigned char*[pyr_num];
		for(int i = 0; i < pyr_num; i++){
			pyr_one[i] = NULL;
			pyr_two[i] = NULL;
		}
		gauss_pyramid(im2, width, height, pyr_two, pyr_num);
		gauss_pyramid(im1, width, height, pyr_one, pyr_num);

		mask_pyr_one = new unsigned char*[pyr_num];
		mask_pyr_two = new unsigned char*[pyr_num];
		for(int i = 0; i < pyr_num; i++){
			mask_pyr_one[i] = NULL;
			mask_pyr_two[i] = NULL;
		}
		gauss_pyramid(mask_two, width, height, mask_pyr_two, pyr_num);
		gauss_pyramid(mask_one, width, height, mask_pyr_one, pyr_num);
	} catch(...){
		cout << "Gauss pyramid block's failed." << endl;
		return 0;
	}

	double old_proj[8];
	memcpy(old_proj, proj, 8 * sizeof(double));
	
	proj[0] = 1.0; proj[1] = 0.0; proj[2] /= pow(2.0, pyr_num - 1);
	proj[3] = 0.0; proj[4] = 1.0; proj[5] /= pow(2.0, pyr_num - 1);
	proj[6] *= 0.0; proj[7] *= 0.0;

	int io_rt[4] = {0, 1, 1, 1};
	int io_pj[4] = {0, 0, 0, 0};

	for(int i = 0; i < 4; i++){
		prepare_proj_picture(pyr_width[i], pyr_height[i], pyr_one[i], pyr_two[i]);

		for (int iter = 1; iter <= iterNum; iter++) {
			prepare_divmask(proj_picture, pyr_two[i], pyr_width[i], pyr_height[i]);
			calculate_maps(proj_picture, pyr_two[i], pyr_width[i], pyr_height[i], iter);
			calculate_matrix(pyr_width[i], pyr_height[i], mask_pyr_one[i], mask_pyr_two[i], iter);
			calculate_q(pyr_width[i], pyr_height[i], 1, io_rt[i], io_pj[i]);
			calculate_proj();
			prepare_proj_picture(pyr_width[i], pyr_height[i], pyr_one[i], pyr_two[i]);
		}

		proj[0] = proj[0]; proj[1] = proj[1]; proj[2] = 2.0 * proj[2];
		proj[3] = proj[3]; proj[4] = proj[4]; proj[5] = 2.0 * proj[5];
		proj[6] = proj[6] * 0.5; proj[7] = proj[7] * 0.5;
	}

	proj[0] = proj[0]; proj[1] = proj[1]; proj[2] = proj[2] * 0.5;
	proj[3] = proj[3]; proj[4] = proj[4]; proj[5] = proj[5] * 0.5;
	proj[6] = proj[6] * 2.0; proj[7] = proj[7] * 2.0;

	if(proj[0] > 1.1 || proj[0] < 0.90 || proj[1] > 0.1 || proj[1] < -0.1 ||
		proj[3] > 0.1 || proj[3] < -0.1 || proj[4] > 1.1 || proj[4] < 0.90){
		proj[0] = 1.0; proj[1] = 0.0; proj[2] = 1.5 * old_proj[2];
		proj[3] = 0.0; proj[4] = 1.0; proj[5] = 1.5 * old_proj[5];
		proj[6] *= 0.0; proj[7] *= 0.0;
	}

	if(proj[2] > BASIC_SIZE / 4)	proj[2] = BASIC_SIZE / 4;
	if(proj[2] < -BASIC_SIZE / 4)	proj[2] = -BASIC_SIZE / 4;
	if(proj[5] > BASIC_SIZE / 4)	proj[5] = BASIC_SIZE / 4;
	if(proj[5] < -BASIC_SIZE / 4)	proj[5] = -BASIC_SIZE / 4;

	prepare_proj_picture(pyr_width[3], pyr_height[3], pyr_one[3], pyr_two[3]);

	
	gauss_pyramid_del(pyr_one, pyr_num);
	gauss_pyramid_del(pyr_two, pyr_num);
	gauss_pyramid_del(mask_pyr_one, pyr_num);
	gauss_pyramid_del(mask_pyr_two, pyr_num);
	delete[] pyr_one;
	delete[] pyr_two;
	delete[] mask_pyr_one;
	delete[] mask_pyr_two;
	delete[] pyr_width;
	delete[] pyr_height;

	return 1;
}
int Projection::gauss_filter(unsigned char* im, int im_width, int im_height)
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
int Projection::gauss_sweep_one(unsigned char** dxx, int im_width, int* gxx)
{
	for(int i = 0; i < im_width; i++)
		gxx[i] = dxx[0][i] + (dxx[1][i] << 2) + (dxx[2][i] << 2) + (dxx[2][i] << 1) +
				(dxx[3][i] << 2) + dxx[4][i];
	return 1;
}
int Projection::gauss_sweep_two(int* gxx, int im_width, int* Gxx)
{
	for(int i = 0; i < im_width - 4; i++)
		Gxx[i] = gxx[i] + (gxx[i + 1] << 2) + (gxx[i + 2] << 2) +
			(gxx[i + 2] << 1) + (gxx[i + 3] << 2) + gxx[i + 4];
	return 1;
}
int Projection::gauss_sweep_three(int* Gxx, int im_width, int* s)
{
	for(int j = 0; j < im_width - 4; j++)
		s[j] = Gxx[j] >> 8;

	return 1;
}
int	Projection::gauss_sweep_four(int * s, int im_width, int im_height, unsigned char* im)
{
	memset(im, 0, im_width * im_height * sizeof(unsigned char));
	for(int j = 2; j < im_height - 2; j++)
		for(int i = 2; i < im_width - 2; i++)
			im[i + j * im_width] = s[i - 2 + (j - 2) * (im_width - 4)];
	return 1;
}
int Projection::calculate_maps(unsigned char* im_one, unsigned char* im_two, 
	int im_width, int im_height, int iteration)
{
	if(!im_one || !im_two || !im_width || !im_height || iteration < 1)
		throw "Calculate maps block receives false data.";

	#pragma omp parallel
	{
		if(iteration == 1){
		memset(Ex, 0, im_width * im_height * sizeof(double));
		memset(Ey, 0, im_width * im_height * sizeof(double));
		memset(Et, 0, im_width * im_height * sizeof(double));
		#pragma omp for	
		for (int j = 1; j < im_height - 1; j++)
			for (int i = 1; i < im_width - 1; i++){
				int coord = i + j * im_width;
				Et[coord] = (im_one[coord] - im_two[coord]);
				Ex[coord] = (im_two[coord + 1] - im_two[coord - 1]) / 2.0;
				Ey[coord] = (im_two[coord + im_width] - im_two[coord - im_width]) / 2.0;
			}
		}else{
			memset(Et, 0, im_width * im_height * sizeof(double));
			#pragma omp for	
			for (int j = 1; j < im_height - 1; j++)
				for (int i = 1; i < im_width - 1; i++){
					int coord = i + j * im_width;
					Et[coord] = (im_one[coord] - im_two[coord]);
			}
		}
	}
	return 1;
}
int Projection::calculate_matrix(int im_width, int im_height, 
	unsigned char* mask_im_one, unsigned char* mask_im_two, int iteration)
{
	if(!Ex || !Ey || !Et || !im_width || !im_height || !mask_im_one || !mask_im_two || !mask_div)
		throw "Calculate matrix block receives false data.";

	int shift = im_width / 10;
	if(iteration == 1){
		memset(A, 0, 64 * sizeof(double));
		memset(C, 0, 8 * sizeof(double));
		#pragma omp parallel
		{
			double t_C[8];
			double t_A[64];
			memset(t_C, 0, 8 * sizeof(double));
			memset(t_A, 0, 64 * sizeof(double));
			#pragma omp for schedule (dynamic, 64)
			for (int j = shift; j < im_height - shift; j++)
				for (int i = shift; i < im_width - shift; i++){
					int c = i + j * im_width;
					if(mask_one[c] > 0 || mask_two[c] > 0 || mask_div[c] > 0)
						continue;

					double x = i - im_width * 0.5;
					double y = j - im_height * 0.5;
					t_A[0] += Ex[c] * x * Ex[c] * x;
					t_A[1] += Ex[c] * x * Ex[c] * y;
					t_A[2] += Ex[c] * x * Ex[c];
					t_A[3] += Ex[c] * x * Ey[c] * x;
					t_A[4] += Ex[c] * x * Ey[c] * y;
					t_A[5] += Ex[c] * x * Ey[c];
					t_A[6] += Ex[c] * x * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[7] += Ex[c] * x * (x * y * Ex[c] + y * y * Ey[c]);

					t_A[8]  += Ex[c] * y * Ex[c] * x;
					t_A[9]  += Ex[c] * y * Ex[c] * y;
					t_A[10] += Ex[c] * y * Ex[c];
					t_A[11] += Ex[c] * y * Ey[c] * x;
					t_A[12] += Ex[c] * y * Ey[c] * y;
					t_A[13] += Ex[c] * y * Ey[c];
					t_A[14] += Ex[c] * y * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[15] += Ex[c] * y * (x * y * Ex[c] + y * y * Ey[c]);

					t_A[16] += Ex[c] * Ex[c] * x;
					t_A[17] += Ex[c] * Ex[c] * y;
					t_A[18] += Ex[c] * Ex[c];
					t_A[19] += Ex[c] * Ey[c] * x;
					t_A[20] += Ex[c] * Ey[c] * y;
					t_A[21] += Ex[c] * Ey[c];
					t_A[22] += Ex[c] * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[23] += Ex[c] * (x * y * Ex[c] + y * y * Ey[c]);

					t_A[24] += Ey[c] * x * Ex[c] * x;
					t_A[25] += Ey[c] * x * Ex[c] * y;
					t_A[26] += Ey[c] * x * Ex[c];
					t_A[27] += Ey[c] * x * Ey[c] * x;
					t_A[28] += Ey[c] * x * Ey[c] * y;
					t_A[29] += Ey[c] * x * Ey[c];
					t_A[30] += Ey[c] * x * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[31] += Ey[c] * x * (x * y * Ex[c] + y * y * Ey[c]);

					t_A[32] += Ey[c] * y * Ex[c] * x;
					t_A[33] += Ey[c] * y * Ex[c] * y;
					t_A[34] += Ey[c] * y * Ex[c];
					t_A[35] += Ey[c] * y * Ey[c] * x;
					t_A[36] += Ey[c] * y * Ey[c] * y;
					t_A[37] += Ey[c] * y * Ey[c];
					t_A[38] += Ey[c] * y * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[39] += Ey[c] * y * (x * y * Ex[c] + y * y * Ey[c]);

					t_A[40] += Ey[c] * Ex[c] * x;
					t_A[41] += Ey[c] * Ex[c] * y;
					t_A[42] += Ey[c] * Ex[c];
					t_A[43] += Ey[c] * Ey[c] * x;
					t_A[44] += Ey[c] * Ey[c] * y;
					t_A[45] += Ey[c] * Ey[c];
					t_A[46] += Ey[c] * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[47] += Ey[c] * (x * y * Ex[c] + y * y * Ey[c]);

					t_A[48] += (x * x * Ex[c] + x * y * Ey[c]) * Ex[c] * x;
					t_A[49] += (x * x * Ex[c] + x * y * Ey[c]) * Ex[c] * y;
					t_A[50] += (x * x * Ex[c] + x * y * Ey[c]) * Ex[c];
					t_A[51] += (x * x * Ex[c] + x * y * Ey[c]) * Ey[c] * x;
					t_A[52] += (x * x * Ex[c] + x * y * Ey[c]) * Ey[c] * y;
					t_A[53] += (x * x * Ex[c] + x * y * Ey[c]) * Ey[c];
					t_A[54] += (x * x * Ex[c] + x * y * Ey[c]) * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[55] += (x * x * Ex[c] + x * y * Ey[c]) * (x * y * Ex[c] + y * y * Ey[c]);

					t_A[56] += (x * y * Ex[c] + y * y * Ey[c]) * Ex[c] * x;
					t_A[57] += (x * y * Ex[c] + y * y * Ey[c]) * Ex[c] * y;
					t_A[58] += (x * y * Ex[c] + y * y * Ey[c]) * Ex[c];
					t_A[59] += (x * y * Ex[c] + y * y * Ey[c]) * Ey[c] * x;
					t_A[60] += (x * y * Ex[c] + y * y * Ey[c]) * Ey[c] * y;
					t_A[61] += (x * y * Ex[c] + y * y * Ey[c]) * Ey[c];
					t_A[62] += (x * y * Ex[c] + y * y * Ey[c]) * (x * x * Ex[c] + x * y * Ey[c]);
					t_A[63] += (x * y * Ex[c] + y * y * Ey[c]) * (x * y * Ex[c] + y * y * Ey[c]);

					t_C[0] += -Et[c] * Ex[c] * x;
					t_C[1] += -Et[c] * Ex[c] * y;
					t_C[2] += -Et[c] * Ex[c];
					t_C[3] += -Et[c] * Ey[c] * x;
					t_C[4] += -Et[c] * Ey[c] * y;
					t_C[5] += -Et[c] * Ey[c];
					t_C[6] += -Et[c] * (x * x * Ex[c] + x * y * Ey[c]);
					t_C[7] += -Et[c] * (x * y * Ex[c] + y * y * Ey[c]);
				}
				#pragma omp critical (crit_projection_firstiter)
				{
					for (int i = 0; i < 8; i++)
						C[i] += t_C[i];
					for(int i = 0 ; i < 64; i++)
						A[i] += t_A[i];
				}
		}
	}else{
		memset(C, 0, 8 * sizeof(double));
		#pragma omp parallel
		{
			double t_C[8];
			memset(t_C, 0, 8 * sizeof(double));
			#pragma omp for schedule (dynamic, 64)
			for (int j = shift; j < im_height - shift; j++)
				for (int i = shift; i < im_width - shift; i++){
					int c = i + j * im_width;
					if(mask_one[c] > 0 || mask_two[c] > 0 || mask_div[c] > 0)
						continue;

					double x = i - im_width * 0.5;
					double y = j - im_height * 0.5;
					C[0] += -Et[c] * Ex[c] * x;
					C[1] += -Et[c] * Ex[c] * y;
					C[2] += -Et[c] * Ex[c];
					C[3] += -Et[c] * Ey[c] * x;
					C[4] += -Et[c] * Ey[c] * y;
					C[5] += -Et[c] * Ey[c];
					C[6] += -Et[c] * (x * x * Ex[c] + x * y * Ey[c]);
					C[7] += -Et[c] * (x * y * Ex[c] + y * y * Ey[c]);
				}
			#pragma omp critical (crit_projection)
				for (int i = 0; i < 8; i++)
					C[i] += t_C[i];
		}
	}
	return 1;
}
int Projection::prepare_proj_picture(int im_width, int im_height, unsigned char* im_one, unsigned char* im_two)
{
	if(!proj_picture || !im_width || !im_height || !im_one || !im_two)
		throw "Prepare projection data failed.";

	if(proj_picture) delete[] proj_picture;
	proj_picture = new unsigned char[im_width * im_height];
	memset(proj_picture, 0, im_width * im_height * sizeof(unsigned char));
	
	#pragma omp parallel
	{
		#pragma omp for
		for(int j = 0; j < im_height; j++)
			for(int i = 0; i < im_width; i++){
				double ip = i - im_width * 0.5;
				double jp = j - im_height * 0.5;
				double norm = 1 / (proj[6] * ip + proj[7] * jp + 1);
				double x = (proj[0] * ip + proj[1] * jp + proj[2]) * norm;
				double y = (proj[3] * ip + proj[4] * jp + proj[5]) * norm;

				x += im_width  * 0.5;
				y += im_height * 0.5;
				if (x < 0 || x > (im_width - 1) || y < 0 || y > (im_height - 1)) {
					proj_picture[i + j * im_width] = 0;
					continue;
				}
				int ix = int(x);
				int iy = int(y);
				double dx = x - ix;
				double dy = y - iy;
				int coord = ix + iy * im_width;
				proj_picture[i + j * im_width] =
					static_cast<unsigned char>((1.0 - dx) * (1.0 - dy) * im_one[coord] +
					(1.0 - dx) * dy * im_one[coord + im_width] +
					dx * (1.0 - dy) * im_one[coord + 1] +
					dx * dy * im_one[coord + 1 + im_width] + 0.5);
			}
	}
	return 1;
}
int Projection::calculate_q(int im_width, int im_height, int io_shift, int io_rotate, int io_pr)
{
	if(!im_width || !im_height)
		throw "Calculate q block failed.";

	double A0[64];
	memcpy(A0, A, 64 * sizeof(double));
	double w[8], u[64], v[64], buffer[3*8];
	SVD(A0, 8, 8, 8, w, u, 8, v, 8, buffer);
	SVBkSb(8, 8, w, u, 8, v, 8, 0, 0, 0, A0, 8, buffer);

	memset(q, 0, 8 * sizeof(double));
	if(io_rotate){
		q[0] = A0[0] * C[0] + A0[1] * C[1] + A0[2] * C[2] + A0[3] * C[3] +
			A0[4] * C[4] + A0[5] * C[5] + A0[6] * C[6] + A0[7] * C[7];
		q[1] = A0[8] * C[0] + A0[9] * C[1] + A0[10] * C[2] + A0[11] * C[3] +
			A0[12] * C[4] + A0[13] * C[5] + A0[14] * C[6] + A0[15] * C[7];
	}
	q[2] = A0[16] * C[0] + A0[17] * C[1] + A0[18] * C[2] + A0[19] * C[3] +
		A0[20] * C[4] + A0[21] * C[5] + A0[22] * C[6] + A0[23] * C[7];
	if(io_rotate){
		q[3] = A0[24] * C[0] + A0[25] * C[1] + A0[26] * C[2] + A0[27] * C[3] +
			A0[28] * C[4] + A0[29] * C[5] + A0[30] * C[6] + A0[31] * C[7];
		q[4] = A0[32] * C[0] + A0[33] * C[1] + A0[34] * C[2] + A0[35] * C[3] +
			A0[36] * C[4] + A0[37] * C[5] + A0[38] * C[6] + A0[39] * C[7];
	}
	q[5] = A0[40] * C[0] + A0[41] * C[1] + A0[42] * C[2] + A0[43] * C[3] +
		A0[44] * C[4] + A0[45] * C[5] + A0[46] * C[6] + A0[47] * C[7];
	if(io_pr){
		q[6] = A0[48] * C[0] + A0[49] * C[1] + A0[50] * C[2] + A0[51] * C[3] +
			A0[52] * C[4] + A0[53] * C[5] + A0[54] * C[6] + A0[55] * C[7];
		q[7] = A0[56] * C[0] + A0[57] * C[1] + A0[58] * C[2] + A0[59] * C[3] +
			A0[60] * C[4] + A0[61] * C[5] + A0[62] * C[6] + A0[63] * C[7];
	}

	double x1 = -im_width * 0.5, y1 = -im_height * 0.5,
			x2 = -im_width * 0.5, y2 = im_height * 0.5,
			x3 = im_width * 0.5, y3 = -im_height * 0.5,
			x4 = im_width * 0.5, y4 = im_height * 0.5;

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
	if(!proj || !q)
		throw "Calculate proj block failed.";

	double proj_new[8];
	double denom = proj[6] * q[2] + proj[7] * q[5] + 1;

	if(!denom)
		throw "Calculate proj block, zero denominator.";

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
int Projection::load_mask_one(unsigned char* mask)
{
	if(!mask || !width || !height || !mask_one)
		return 0;
	memcpy(mask_one, mask, width * height * sizeof(unsigned char));
	return 1;
}
int Projection::load_mask_two(unsigned char* mask)
{
	if(!mask || !width || !height || !mask_two)
		return 0;
	memcpy(mask_two, mask, width * height * sizeof(unsigned char));
	return 1;
}
int Projection::clear_data()
{
	proj[0] = 1.0; proj[1] = 0.0; proj[2] = 0.0;
	proj[3] = 0.0; proj[4] = 1.0; proj[5] = 0.0;
	proj[6] = 0.0; proj[7] = 0.0;

	if(im1)				{delete[] im1; im1 = NULL;}
	if(im2)				{delete[] im2; im2 = NULL;}
	if(mask_one)		{delete[] mask_one; mask_one = NULL;}
	if(mask_two)		{delete[] mask_two; mask_two = NULL;}
	if(mask_div)		{delete[] mask_div; mask_div = NULL;}
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
int Projection::manual_reset(unsigned char* im_one, unsigned char* im_two, int im_width, int im_height, double* pr)
{
	if(!im_one || !im_two || !im_width || !im_height || !pr)
		throw "Manual reset block's failed.";
	
	clear_data();

	memcpy(proj, pr, 8 * sizeof(double));

	width = im_width;
	height = im_height;
	
	im2 = new unsigned char[width * height];
	memcpy(im2, im_two, width * height * sizeof(unsigned char));
	for(int i = 0; i < gauss_depth; i++)
		gauss_filter(im2, width, height);

	im1 = new unsigned char[width * height];
	memcpy(im1, im_one, width * height * sizeof(unsigned char));
	for(int i = 0; i < gauss_depth; i++)
		gauss_filter(im1, width, height);

	mask_one = new unsigned char[width * height];
	memset(mask_one, 0, width * height * sizeof(unsigned char));
			
	mask_two = new unsigned char[width * height];
	memset(mask_two, 0, width * height * sizeof(unsigned char));

	proj_picture = new unsigned char[width * height];
	memset(proj_picture, 0, width * height * sizeof(unsigned char));
			
	Ex = new double[width * height];
	memset(Ex, 0, width * height * sizeof(double));
	
	Ey = new double[width * height];
	memset(Ey, 0, width * height * sizeof(double));

	Et = new double[width * height];
	memset(Et, 0, width * height * sizeof(double));

	return 1;
}
int Projection::gauss_pyramid(unsigned char* im, int im_width, int im_height,
		unsigned char** pyramid, int pyramid_num)
{
	if(!im || !im_width || !im_height || !pyramid || pyramid_num < 1)
		throw "Gauss pyramid receives false data.";
	if(pyramid[pyramid_num - 1]) delete[] pyramid[pyramid_num - 1];
	pyramid[pyramid_num - 1] = new unsigned char[im_width * im_height];
	memcpy(pyramid[pyramid_num - 1], im, im_width * im_height * sizeof(unsigned char));
	
	int w = im_width;
	int h = im_height;
	for(int i = pyramid_num - 1; i > 0; i--){
		if(!w || !h){pyramid[i - 1] = NULL; continue;}
		pyramid_step(pyramid[i], pyramid[i - 1], w, h);
		w /= 2;
		h /= 2;
	}

	if(!w || !h)
		throw "Gauss pyramid is to high.";

	return 1;
}
int Projection::pyramid_step(unsigned char* im, unsigned char* &im_step, int im_width, int im_height)
{
	if(!im || !im_width || !im_height)
		throw "Gauss step receives false data.";
	int step_width = im_width / 2;
	int step_height = im_height / 2;
	if(!step_width || !step_height)
		throw "Gauss step produce false data.";
	if(im_step) delete[] im_step;
	im_step = new unsigned char[step_width * step_height];
	
	unsigned char* temp = new unsigned char[im_width * im_height];
	memcpy(temp, im, im_width * im_height * sizeof(unsigned char));
	gauss_filter(temp, im_width, im_height);
	
	for(int j = 0; j < step_height; j++)
		for(int i = 0; i < step_width; i++)
			im_step[i + j * step_width] = temp[2 * (i + j * im_width)];
		
	delete[] temp;
	return 1;
}
int Projection::prepare_divmask(unsigned char* im_one, unsigned char* im_two, int im_width, int im_height)
{
	if(!im_one || !im_two || !im_width || !im_height)
		throw "Prepare divmask block receives false data.";
	if(mask_div) delete[] mask_div;
	mask_div = new unsigned char[im_width * im_height];
	memset(mask_div, 0, im_width * im_height * sizeof(unsigned char));

	
	for(int j = DIV_MASK_BORDER; j < im_height - DIV_MASK_BORDER; j++)
		for(int i = DIV_MASK_BORDER; i < im_width - DIV_MASK_BORDER; i++){
			int coord = i + j * im_width;
			if(!im_one[coord] || !im_two[coord])
				for(int y = -DIV_MASK_BORDER; y <= DIV_MASK_BORDER; y++)
					for(int x = -DIV_MASK_BORDER; x <= DIV_MASK_BORDER; x++)
						mask_div[coord + x + y * im_width] = 255;
		}
	return 1;
}
int Projection::save_proj_result(double* pr)
{
	if(!pr)
		throw "Save proj block's failed.";
	memcpy(pr, proj, 8 * sizeof(double));
	return 1;
}
int Projection::gauss_pyramid_del(unsigned char** pyramid, int pyramid_num)
{
	for(int i = 0; i < pyramid_num; i++){
		if(pyramid[i]) delete[] pyramid[i];
		pyramid[i] = NULL;
	}
	return 1;
}
