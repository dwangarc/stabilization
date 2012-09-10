#pragma once

#ifdef _DEBUG
#pragma comment (lib, "libabacus_d")
#else
#pragma comment (lib, "libabacus")
#endif

typedef void (*abacus_info)(const char *szFormat, ...);

// returns supported instruction set for the current CPU
#define _CPU_FEATURE_MMX	0x0001
#define _CPU_FEATURE_SSE	0x0002
#define _CPU_FEATURE_SSE2	0x0004
#define _CPU_FEATURE_SSE3	0x0008
#define _CPU_FEATURE_SSSE3	0x0010
#define _CPU_FEATURE_SSE41	0x0020
#define _CPU_FEATURE_SSE42	0x0040
int		cpu_instruction_set(void);

// find real roots of polynomials
int		solve_cubic		(double *coeffs, int coeff_count, double *roots);
int		solve_quartic	(double *coeffs, int coeff_count, double *roots);
int		solve_sturm		(double *coeffs, int coeff_count, double *roots);

// fit data with polynomial of order n (err_y - square root of measurement variance)
int		fit_poly(double *x, double *y, double *err_y, int count, double *a, int n);

// rand and median selection
void	select_hoare_32f(float *arr, int n, int rank);
float	select_bfprt_32f(float *arr, int n, int rank);
void	select_indexed_hoare_32f(float *arr, int *idx, int n, int rank);

// Decomposes a=u*w*vT
// a	- matrix to decompose
// lda	- step in matrix a in doubles
// m	- rows in matrix a
// n	- columns in matrix a
// w	- destination matrix
// uT	- destination matrix
// lduT	- step in matrix uT in doubles
// vT	- destination matrix
// ldvT	- step in matrix vT in doubles
// Required buffer size: 2*n+m in doubles
void	SVD(double* a, int lda, int m, int n, double* w, double* uT, int lduT, double* vT, int ldvT, double* buffer);
void	SVBkSb(int m, int n, double* w, double* uT, int lduT, double* vT, int ldvT, double* b, int ldb, int nb, double* x, int ldx, double* buffer);
int		la_solve_lu(double *a, double *b, double *x, int m, void *buf);

// calculates (src-delta)T*(src-delta)
void	mul_transposed_r(double* src, int srcstep, double* dst, int dststep, double* delta, int deltastep, int cols, int rows, int delta_cols, double scale);
// calculates (src-delta)*(src-delta)T
void	mul_transposed_l(double* src, int srcstep, double* dst, int dststep, double* delta, int deltastep, int cols, int rows, int delta_cols, double scale);

// convolution filters
void	filter3x3_vert(unsigned char *src, int src_step, short *dst, int dst_step, int width, int height, int cn);
void	filter3x3_horz(unsigned char *src, int src_step, short *dst, int dst_step, int width, int height, int cn);
void	filter3x3_diagu(unsigned char *src, int src_step, short *dst, int dst_step, int width, int height, int cn);
void	filter3x3_diagd(unsigned char *src, int src_step, short *dst, int dst_step, int width, int height, int cn);

void	filter_symm_32f(const float *src, int src_step, float *dst, int dst_step, int width, int height, int cn, float *kx, int xsize, float *ky, int ysize);
void	filter_box_32f(const float *src, int src_step, float *dst, int dst_step, int width, int height, int cn, int xsize, int ysize);
void	filter_box_32s(const int *src, int src_step, int *dst, int dst_step, int width, int height, int cn, int xsize, int ysize);
void	filter_box_16u(const unsigned short *src, int src_step, unsigned short *dst, int dst_step, int width, int height, int cn, int xsize, int ysize);
void	filter_min_32f(const float *src, int src_step, float *dst, int dst_step, int width, int height, int cn, int xsize, int ysize);
void	filter_min_16u(const unsigned short *src, int src_step, unsigned short *dst, int dst_step, int width, int height, int cn, int xsize, int ysize);
void	filter_box_scale_8u(const unsigned char *src, int src_step, unsigned char *dst, int dst_step, int width, int height, int cn, int xsize, int ysize);

void	filter_gauss_32f(const float *src, int src_step, float *dst, int dst_step, int width, int height, int cn, double sigma, int use_sse);

// interpolation
float	interpolate_bicubic_2d(const float *arr, int step, double x, double y, int width, int height);
double	interpolate_bicubic_2d(const unsigned char *arr, int step, double x, double y, int width, int height);
void	interpolate_bicubic_2d_cn(const unsigned char *arr, int step, double x, double y, int width, int height, int cn, double *res);
void	interpolate_bicubic_2d_cn(const float *arr, int step, double x, double y, int width, int height, int cn, double *res);

// resizing
int		resize_nn_8u		(const unsigned char *src, int sstep, int swidth, int sheight, unsigned char *dst, int dstep, int dwidth, int dheight, int cn);
int		resize_bilinear_8u	(const unsigned char *src, int sstep, int swidth, int sheight, unsigned char *dst, int dstep, int dwidth, int dheight, int cn);
int		resize_bicubic_8u	(const unsigned char *src, int sstep, int swidth, int sheight, unsigned char *dst, int dstep, int dwidth, int dheight, int cn);
int		resize_area_8u		(const unsigned char *src, int sstep, int swidth, int sheight, unsigned char *dst, int dstep, int dwidth, int dheight, int cn);
void	resize_nn_32f		(const float *src, int sstep, int swidth, int sheight, float *dst, int dstep, int dwidth, int dheight);
void	resize_bicubic_32f	(const float *src, int sstep, int swidth, int sheight, float *dst, int dstep, int dwidth, int dheight);

int		map_affine_bilinear_8u(unsigned char *src, int sstep, int swidth, int sheight, unsigned char *dst, int dstep, int dwidth, int dheight, int cn, double *m);

// morphological operations
int		binary_erode_8u(unsigned char *data, int step, int width, int height, int size, void *buffer);
int		binary_dilate_8u(unsigned char *data, int step, int width, int height, int size, void *buffer);
void	BinaryUnion8u(unsigned char *data, unsigned char *ref, int width, int height);
void	HalftoneErode8u(unsigned char *data, int width, int height, int size);
void	HalftoneDilate8u(unsigned char *data, int width, int height, int size);

#define mat2x2_det(m) ((m)[0]*(m)[3] - (m)[1]*(m)[2])
#define mat3x3_det(m) ((m)[0]*((m)[4]*(m)[8] - (m)[5]*(m)[7]) - (m)[1]*((m)[3]*(m)[8] - (m)[5]*(m)[6]) + (m)[2]*((m)[3]*(m)[7] - (m)[4]*(m)[6]))
#define mat4x4_det(m) \
			(((m)[0] * (m)[5] - (m)[1] * (m)[4]) * ((m)[10] * (m)[15] - (m)[11] * (m)[14]) - \
			 ((m)[0] * (m)[6] - (m)[2] * (m)[4]) * ((m)[ 9] * (m)[15] - (m)[11] * (m)[13]) + \
			 ((m)[0] * (m)[7] - (m)[3] * (m)[4]) * ((m)[ 9] * (m)[14] - (m)[10] * (m)[13]) + \
			 ((m)[1] * (m)[6] - (m)[2] * (m)[5]) * ((m)[ 8] * (m)[15] - (m)[11] * (m)[12]) - \
			 ((m)[1] * (m)[7] - (m)[3] * (m)[5]) * ((m)[ 8] * (m)[14] - (m)[10] * (m)[12]) + \
			 ((m)[2] * (m)[7] - (m)[3] * (m)[6]) * ((m)[ 8] * (m)[13] - (m)[ 9] * (m)[12]))


void	mat3x3_ident(double *m);
void	mat3x3_translate(double *m, double tx, double ty);
void	mat3x3_scale(double *m, double sx, double sy);
void	mat3x3_rotate(double *m, double a);
void	mat3x3_skew(double *t, double *t_x);
int		mat3x3_invert(double *H, double *Hinv);
int		mat4x4_invert(double *H, double *Hinv);
void	mat3x3_transpose(double *M, double *res);
void	mat3x3_mul(double *A, double *B, double *res);
void	mat3x3_mulleft(double *A, double *L, bool transposeL = false);
void	mat3x3_mulright(double *A, double *R, bool transposeR = false);
void	mat3x3_mulvec(double *A, double *src, double *dst, bool transposeA = false);
int		mat3x3_decompose_rq(double *M, double *R, double *Q);

void	rodrigues_mat2vec(double A[9], double v[3]);
void	rodrigues_vec2mat(double v[3], double R[9], double J[27] = 0);

// Bezier curves
void	Bezier3_point(double *cp, double &px, double &py, double u);
double	Bezier3_distance_sq(double *cp, double px, double py, double *pu = 0);
double	Bezier3_fit_free(double *points, int count, double *cp);
double	Bezier3_fit_fixed(double *points, int count, double *cp);

// sets routine for debug messages
void	mview_set_info(abacus_info info, int verbose);

// transforms estimation
int		homography_2d_estimate_dlt		(double *H, const double *m0, const double *m1, int count, bool *mask);
int		homography_2d_estimate_ransac	(double *H, const double *m0, const double *m1, int count, bool *mask, double threshold, double p);
int		similarity_2d_estimate_lls		(double *H, const double *m0, const double *m1, int count, bool *mask);
int		similarity_2d_estimate_ransac	(double *H, const double *m0, const double *m1, int count, bool *mask, double threshold, double p);
int		affine_2d_estimate_lls			(double *H, const double *m0, const double *m1, int count, bool *mask);
int		homography_2d_update_mask		(double *H, const double *m0, const double *m1, int count, bool *mask, double threshold);	// updates point mask based on specified threshold; returns number of changed elements

int		homography_3d_estimate_dlt		(double *H, const double *m0, const double *m1, int count, bool *mask);
int		similarity_3d_estimate			(double *H, const double *m0, const double *m1, int count, bool *mask);

// fundamental matrix estimation
int		fmatrix_estimate_7point	(double *F, const double *m0, const double *m1);
int		fmatrix_estimate_8point	(double *F, const double *m0, const double *m1, int count, bool *mask);
int		fmatrix_estimate_ransac	(double *F, const double *m0, const double *m1, int count, bool *mask, double threshold, double p);
int		fmatrix_estimate_lmeds	(double *F, const double *m0, const double *m1, int count, bool *mask, double threshold, double p);
int		fmatrix_refine_algebraic(double *F, const double *m0, const double *m1, int count, bool *mask, double eps, int max_iter);
int		fmatrix_refine_geometric(double *F, const double *m0, const double *m1, int count, bool *mask, double eps, int max_iter);
int		fmatrix_update_mask		(double *F, const double *m0, const double *m1, int count, bool *mask, double threshold);	// updates point mask based on specified threshold; returns number of changed elements
void	fmatrix_compose			(double *F, double *P0, double *P1);
void	fmatrix_decompose		(double *F, double *P0, double *P1);
int		fmatrix_decompose_metric(double *F, double *P0, double *P1, double *A0, double *A1, double *m0, double *m1, int count, bool *mask);

void	fmatrix_epipoles		(double *F, double *e1, double *e2);			// compute epipoles on the left and right frame
void	fmatrix_epiline_left	(double *F, double x1, double x2, double *l);	// returns epiline on the left frame matching the specified point from the right frame	[ l = Ftx' ]
void	fmatrix_epiline_right	(double *F, double x1, double x2, double *l);	// returns epiline on the right frame matching the specified point from the left frame	[ l' = Fx ]

// essential matrix estimation
int		ematrix_estimate_5point	(double *E, const double *m0, const double *m1);
int		ematrix_estimate_ransac	(double *E, const double *m0, const double *m1, int count, bool *mask, double thresh0, double thresh1, double p);
//int	ematrix_refine_geometric(double *E, const double *m0, const double *m1, int count, bool *mask, double eps, int max_iter);
//int	ematrix_update_mask		(double *E, const double *m0, const double *m1, int count, bool *mask, double threshold);	// updates point mask based on specified threshold; returns number of changed elements
void	ematrix_compose_rt		(double *E, double *R, double *T);

// trifocal tensor estimation
int		ttensor_estimate_6point	(double *T, const double *m0, const double *m1, const double *m2);
int		ttensor_estimate_7point	(double *T, const double *m0, const double *m1, const double *m2, int count, bool *mask);
int		ttensor_estimate_ransac	(double *T, const double *m0, const double *m1, const double *m2, int count, bool *mask, double threshold, double p);
int		ttensor_refine_geometric(double *T, const double *m0, const double *m1, const double *m2, int count, bool *mask, double eps, int max_iter);
int		ttensor_update_mask		(double *T, const double *m0, const double *m1, const double *m2, int count, bool *mask, double threshold);	// updates point mask based on specified threshold; returns number of changed elements
void	ttensor_compose			(double *T, double *P0, double *P1, double *P2);
void	ttensor_decompose		(double *T, double *P0, double *P1, double *P2);
int		ttensor_transform		(double *T, double *H0, double *H1, double *H2);
int		ttensor_decompose_metric(double *T, double *P0, double *P1, double *P2, double *A0, double *A1, double *A2, double *m0, double *m1, double *m2, int count, bool *mask);

int		motion2_estimate_pt_dlt	(double *R, double *T, const double *m0, const double *m1, double f0, double f1, double *X);
int		motion2_estimate_ematrix(double *R, double *T, const double *m0, const double *m1, int count, bool *mask, double f0, double f1, double *E);
int		motion2_refine_geometric(double *R, double *T, const double *m0, const double *m1, int count, bool *mask, double f0, double f1, double eps, int max_iter);
int		motion2_update_mask		(double *R, double *T, const double *m0, const double *m1, int count, bool *mask, double f0, double f1, double thresh);

int		motion3_refine_geometric(double *R1, double *O1, double *R2, double *O2, const double *m0, const double *m1, const double *m2, int count, bool *mask, double eps, int max_iter);
int		motion3_update_mask		(double *P0, double *P1, double *P2, const double *m0, const double *m1, const double *m2, int count, bool *mask, double threshold);
int		camera_2view_canonicalize(double *P0, double *P1);
int		camera_3view_canonicalize(double *P0, double *P1, double *P2);
int		camera_2view_refine_geometric(double *P0, double *P1, const double *m0, const double *m1, int count, bool *mask, double eps, int max_iter);
int		camera_3view_refine_geometric(double *P0, double *P1, double *P2, const double *m0, const double *m1, const double *m2, int count, bool *mask, double eps, int max_iter);
int		camera_2view_update_mask(double *P0, double *P1, const double *m0, const double *m1, int count, bool *mask, double threshold);
int		camera_3view_update_mask(double *P0, double *P1, double *P2, const double *m0, const double *m1, const double *m2, int count, bool *mask, double threshold);

// camera resectioning
int		camera_estimate_3point	(double *P, const double *M, const double *m, double *A);
int		camera_estimate_dlt		(double *P, const double *M, const double *m, int count, bool *mask);
int		camera_estimate_ransac	(double *P, const double *M, const double *m, double *A, int count, bool* mask, double threshold, double p);
int		camera_refine_geometric	(double *P, const double *M, const double *m, int count, bool *mask, double eps, int max_iter);
int		camera_update_mask		(double *P, const double *M, const double *m, int count, bool *mask, double threshold);	// updates point mask based on specified threshold; returns number of changed elements
int		camera_compose			(double *P, double *A, double *R, double *O);
int		camera_decompose		(double *P, double *A, double *R, double *O);
int		camera_center			(double *P, double *O);
int		camera_estimate_auto	(const double *M, const double *m, int count, double *A, double *R, double *O, bool *mask);

// structure computation (X in homogeneous coordinates)
int		point_2view_estimate_dlt(double *X, double *P0, double *P1, const double *m0, const double *m1);
int		point_2view_estimate_inh(double *X, double *P0, double *P1, const double *m0, const double *m1);
int		point_2view_estimate_opt(double *X, double *P0, double *P1, const double *m0, const double *m1);
int		point_3view_estimate_dlt(double *X, double *P0, double *P1, double *P2, const double *m0, const double *m1, const double *m2);
int		point_3view_estimate_inh(double *X, double *P0, double *P1, double *P2, const double *m0, const double *m1, const double *m2);
int		point_nview_estimate_dlt(double *X, double *P, double *v, int count);
int		point_2view_refine_geometric(double *X, double *P0, double *P1, const double *m0, const double *m1, double eps, int max_iter);
int		point_3view_refine_geometric(double *X, double *P0, double *P1, double *P2, const double *m0, const double *m1, const double *m2, double eps, int max_iter);
int		point_nview_refine_geometric(double *X, double *P, double *v, int count, double eps, int max_iter);

void	estimate_homographies_hartley(double *fmatrix, double *pt1, double *pt2, bool *mask, int pt_count, int width, int height, double *H1, double *H2);
void	estimate_homographies_whelan(double *fmatrix, int width, int height, double *H1, double *H2);
double	estimate_homographies_metric(double *ematrix, int width, int height, double *A0, double *A1, double *H0, double *H1);
