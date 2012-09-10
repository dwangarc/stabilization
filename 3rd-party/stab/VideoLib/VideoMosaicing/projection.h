//just load_im() twice then take_proj(), projection[] will contain all the data You need
//Steve Mann, "Video Orbits of the Projective Group"
/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Give projection in the system where center of the image has coordinate (0, 0)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/
class Projection {

public:
	Projection();
	~Projection();

	int load_im(unsigned char* im, int im_width, int im_height); //load im2, old im2 replace im1 (prepare copy of input data)
	int load_mask_one(unsigned char* mask);//load mask_one and mask_two
	int load_mask_two(unsigned char* mask);
	
	int save_proj_result(double* pr);
	unsigned char* take_picture_projection();
	unsigned char* take_picture_two();

	int change_iterNum(int new_iterNum);
	int change_gaussDepth(int new_gaussDepth);
	int multiply_projection(double* lp, double* rp, double* result);
	int manual_reset(unsigned char* im_one, unsigned char* im_two, int im_width, int im_height, double* pr);

	int reset();

	double proj[8];//contain eight parameters of PT

protected:
	
	unsigned char* im1;//internal im1
	unsigned char* im2;//internal im2
	unsigned char* proj_picture;
	
	int width;//width of images
	int height;//height of images	

	unsigned char* mask_one;//mask
	unsigned char* mask_two;
	unsigned char* mask_div;

	int gauss_depth;//the number of gauss preconditionings
	int iterNum;//iter Number

	double* Ex;//internal maps
	double* Ey;
	double* Et;
	
	double A[64];//calculate A (\phi X \phi) and C (right part of the matrix equation) matrix
	double C[8];
	double q[8];//temp buffer for 

protected:
	int gauss_filter(unsigned char* im, int im_width, int im_height);
	int gauss_sweep_one(unsigned char** dxx, int im_width, int* gxx);
	int gauss_sweep_two(int* gxx, int im_width, int* Gxx);
	int gauss_sweep_three(int* Gxx, int im_width, int* s);
	int	gauss_sweep_four(int * s, int im_width, int im_height, unsigned char* im);

	int gauss_pyramid(unsigned char* im, int im_width, int im_height,
		unsigned char** pyramid, int pyramid_num);
	int pyramid_step(unsigned char* im, unsigned char* &im_step, int im_width, int im_height);
	int gauss_pyramid_del(unsigned char** pyramid, int pyramid_num);

	int prepare_divmask(unsigned char* im_one, unsigned char* im_two, int im_width, int im_height);
	int calculate_maps(unsigned char* im_one, unsigned char* im_two, 
		int im_width, int im_height, int iteration);
	int calculate_matrix(int im_width, int im_height, 
		unsigned char* mask_im_one, unsigned char* mask_im_two, int iteration);

	int prepare_proj_picture(int im_width, int im_height, unsigned char* im_one, unsigned char* im_two);
	int calculate_q(int im_width, int im_height, int io_shift, int io_rotate, int io_pr);
	int calculate_proj();

	int clear_data();
};