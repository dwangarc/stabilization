//just load_im() twice then take_proj(), projection[] will contain all the data You need
//Steve Mann, "Video Orbits of the Projective Group"
/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Give projection in the system where center of the image has coordinate (0, 0)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

#include "Projective/ProjectiveConverter.h"

class Projection {

	ProjectiveConverter converter;


public:
	Projection();
	~Projection();

//	int load_im(unsigned char* im, int im_width, int im_height); //load im2, old im2 replace im1 (prepare copy of input data)

	void initNonPic(int width, int height);
	int estimateProj(unsigned char* image1, unsigned char* image2, int iterNum, double const* proj = 0);

	double proj[8];//contain eight parameters of PT

	//int load_mask_one(unsigned char* mask);//load mask_one and mask_two
	//int load_mask_two(unsigned char* mask);
	unsigned char* take_picture_projection();
	unsigned char* take_picture_two();
	int gauss_filter(unsigned char* im, int im_width, int im_height) const;//gauss filter	

	int change_iterNum(int new_iterNum);
	int change_gaussDepth(int new_gaussDepth);
	int multiply_projection(double* lp, double* rp, double* result);

	int reset();

	static void initProj(double* proj, bool nine = false);

	int getWidth() const;
	int getHeight() const;

protected:
	unsigned char* im1;//internal im1
	unsigned char* im2;//internal im2
	//unsigned char* mask_one;//mask
	//unsigned char* mask_two;

	int width;//width of images
	int height;//height of images

	//double* st[11];

	double tau;

	int gauss_depth;//the number of gauss preconditionings
	int iterNum;//iter Number

	inline int gauss_sweep_one(unsigned char** dxx, int im_width, int* gxx) const;
	inline int gauss_sweep_two(int* gxx, int im_width, int* Gxx) const;
	inline int gauss_sweep_three(int* Gxx, int im_width, int* s) const;
	inline int gauss_sweep_four(int * s, int im_width, int im_height, unsigned char* im) const;

	int* Ex;//internal maps
	int* Ey;
	int*	Et;
	double A[64];//calculate A (\phi X \phi) and C (right part of the matrix equation) matrix
	double C[8];
	double q[8];//temp buffer for 
	unsigned char* proj_picture;
	
	int calculate_maps(int iteration);//calculate internal maps, takes number of iteration 
	int calculate_matrix(int iteration);
	int calculate_q();//calculate q (eight projective parameters)
	void estimate_q_fromAC(double const* A0, double const* C0);
	int calculate_proj();
	int prepare_proj_picture();//calculate transformed image

	int mat3x3_invert(double *m, double *m_inv);

	double calculateDerX(int x, int y) const;
	double calculateDerY(int x, int y) const;
	void fixTATC(double* t_A, double *t_C) const;

	
	int clear_data();
};