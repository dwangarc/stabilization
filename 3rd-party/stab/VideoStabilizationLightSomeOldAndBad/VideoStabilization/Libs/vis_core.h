/***************************************************** Image processing *****************************************************/

// color definitions
#define COLOR_AUTO			0
#define COLOR_MONO			1
#define COLOR_RGB			2
#define COLOR_LHS			3
#define COLOR_YCbCr			4
#define COLOR_BGR			5
#define COLOR_BGR_INV		6

// image in Windows bitmap format
typedef struct bitmap {
	int						width;
	int						height;
	int						step;
	unsigned char *			data;
	struct tagBITMAPINFO *	pbmi;
} bitmap;

// general floating point matrix
typedef struct mat32f {
	int						width;
	int						height;
	int						cn;
	int						step;
	float *					data;
} mat32f;

// general uchar matrix
typedef struct mat8u {
	int						width;
	int						height;
	int						cn;
	int						step;
	unsigned char *			data;
} mat8u;

int				image_count(const char *folder);
int				image_validate(const char *path);
int				image_dims(const char *path, int &width, int &height);

bitmap *		bitmap_create(int width, int height, int planes, bitmap *bmp = 0);
void			bitmap_release(bitmap *bmp);
bitmap *		bitmap_copy(bitmap *src, bitmap *dst = 0);
void			bitmap_init(bitmap *bmp, struct tagBITMAPINFO *pbmi, unsigned char *data);
bitmap *		bitmap_from_mat8u(mat8u *image, int color, bitmap *bmp = 0);
void			bitmap_setpixel(bitmap *bmp, int x, int y, unsigned long c);
unsigned long	bitmap_getpixel(bitmap *bmp, int x, int y);
unsigned long	bitmap_getpixel_bilinear(bitmap *bmp, double nx, double ny);
unsigned long	bitmap_getpixel_bicubic(bitmap *bmp, double nx, double ny);
void			bitmap_line(bitmap *bmp, int x0, int y0, int x1, int y1, unsigned long c);
bitmap*			bitmap_load_bmp(const char *path, int planes);
bitmap*			bitmap_load_jpeg(const char *path, int planes);
bitmap*			bitmap_load_pnm(const char *path, int planes);
bitmap*			bitmap_load_sgi(const char *path, int planes);
int				bitmap_save_jpeg(bitmap *bmp, const char *path);
bitmap *		bitmap_load(const char *path, int planes = 0);
int				bitmap_save(bitmap *bmp, const char *path);

mat8u *			mat8u_create(int width, int height, int channels, mat8u *mat = 0);
void			mat8u_release(mat8u *mat);
mat8u *			mat8u_copy(mat8u *src, mat8u *dst = 0);
mat8u *			mat8u_from_bitmap(bitmap *bmp, int color, mat8u *image = 0);
void			mat8u_flip(mat8u *image);
void			mat8u_setpixel(mat8u *image, int x, int y, unsigned long c);
unsigned long	mat8u_getpixel(mat8u *image, int x, int y);
unsigned long	mat8u_getpixel_bilinear(mat8u *image, double nx, double ny);
unsigned long	mat8u_getpixel_bicubic(mat8u *image, double nx, double ny);
mat8u *			mat8u_load_image(const char *path, int color);
int				mat8u_save_image(mat8u *image, const char *path, int color = COLOR_AUTO);
int				mat8u_save_region(mat8u *image, const char *path, int color, int x, int y, int w, int h, int a);

mat32f *		mat32f_create(int width, int height);
void			mat32f_release(mat32f *patf);
mat32f *		mat32f_from_mat8u(mat8u *src, float factor, mat32f *dst = 0);
void			mat32f_smooth_gaussian(mat32f *src, mat32f *dst, double sigma);
void			mat32f_subtract(mat32f *src1, mat32f *src2, mat32f *dst);
mat32f *		mat32f_copy(mat32f *src);
void			mat32f_resize_nn(mat32f *src, mat32f *dst);
void			mat32f_resize_bicubic(mat32f *pat_src, mat32f *pat_dst);
double			mat32f_getpixel_bicubic(mat32f *mat, double nx, double ny);

void			bitmap_auto_levels(bitmap *bmp);
void			bitmap_equalize_gray_world(bitmap *bmp);
void			bitmap_equalize_GIMP(bitmap *bmp);

/*********************************************** Fundamental matrix computation *********************************************/

#define FMATRIX_ALGEBRAIC	0x01
#define FMATRIX_GEOMETRIC	0x02
#define FMATRIX_RANSAC		0x10
#define FMATRIX_LMEDS		0x20
#define FMATRIX_GUIDED		0x40

// m0		- first camera point coordinates
// m1		- second camera point coordinates
// count	- points count
// F		- output fundamental matrix
// method	- optional RANSAC flag
// thres	- maximum matching threshold (RANSAC only)
// p		- desirable level of confidence (RANSAC only)
// mask		- marks points used for computation. RANSAC method fills this array (optional)
int compute_fundamental(const double* m0, const double* m1, int count, double *F, int method, bool* mask = 0, double thres = 1.0, double p = 1.0);

// returns epiline on the right frame matching the specified point from the left frame	[ l' = Fx ]
void fundamental_epiline_right(const double *F, double x1, double x2, double *l);

// returns epiline on the left frame matching the specified point from the right frame	[ l = Ftx' ]
void fundamental_epiline_left(const double *F, double x1, double x2, double *l);

// compute epipoles on the left and right frame
void fundamental_epipoles(const double *F, double *e1, double *e2);

/**************************************************** Rectification **********************************************************/

void	compute_homographies_hartley(double *fmatrix, double *pt1, double *pt2, bool *mask, int pt_count, int width, int height, double *H1, double *H2);
void	compute_homographies_whelan(double *fmatrix, int width, int height, double *H1, double *H2);
double	compute_homographies_metric(double *fmatrix, int width, int height, double *ic1, double *ic2, double *H1, double *H2);

struct stereo_rect;

// initializes rectification object
// src		- source image buffer
// width	- image width
// height	- image height
// cn		- number of channels
// src_step	- row stride in the source image
// dst_step	- row stride in the destination image
// H		- rectification homography matrix
// ic		- intrinsic camera parameters (fx, fy, cx, cy, K0, K1, K2, K3)
// mode		- interpolation mode (0 - nearest neighbour, 1 - bilinear, 2 - bicubic)
stereo_rect *	stereo_rect_init(int width, int height, int scanlength, int color_space_src, int cn_to, int scale, double *H, double *ic);

// rectifies image
// src		- source buffer
// dst		- destination buffer
void			stereo_rect_process(stereo_rect *rect, unsigned char *src, unsigned char *dst);

// releases rectification object
void			stereo_rect_release(stereo_rect *rect);

/************************************************* Image preprocessing ********************************************************/

typedef struct stereo_preproc_params {
	int		filter_hi;			// 0 - None, 1 - Gauss 1x3, 2 - Gauss 3x5
	int		filter_lo;			// 0 - None, 1 - Box, 2 - Bilateral
	int		win_size;			// window size
	double	gamma_proximity;	// similarity gamma (bilateral filtering)
	double	gamma_similarity;	// proximity gamma (bilateral filtering)
} stereo_preproc_params;

struct stereo_preproc;

stereo_preproc *	stereo_preproc_create(int width, int height, int cn, stereo_preproc_params *params);
void				stereo_preproc_process(stereo_preproc *preproc, unsigned char *im);
void				stereo_preproc_release(stereo_preproc *preproc);

/************************************************* Depth estimation **********************************************************/

typedef struct stereo_params {
	int		cost_type;			// 0 - AD, 1 - SD, 2 - Census, 3 - Rank
	int		aggr_type;			// 0 - None, 1 - Box, 2 - ASW, 3 - ASW2
	int		opt_type;			// 0 - WTA, 1 - SO, 2 - DP, 3 - RDP, 4 - cvDP, 5 - ORDP, 6 - ICM, 7 - TRWS, 8 - BPS, 9 - BPM, 10 - Expansion, 11 - Swap

	int		win_size_x;			// aggregation window size x
	int		win_size_y;			// aggregation window size y
	int		min_filter;			// optional spatial min-filter after aggregation (shiftable window)
	int		win_census;			// window size for census cost

	int		disp_min;
	int		disp_num;
	int		med_filter;
	int		match_max;
	double	gamma_similarity;	// asw similarity gamma
	double	gamma_proximity;	// asw proximity gamma

	int		opt_smoothness;		// smoothness penalty multiplier (lambda)
	int		opt_grad_thresh;	// threshold for magnitude of intensity gradient
	int		opt_grad_penalty;	// smoothness penalty factor if gradient is too small
	int		opt_occlusion_cost;	// cost for occluded pixels in DP algorithm
	int		opt_lr_check;		// left-right checking (WTA only)
	int		opt_reliability;	// required reliability (WTA and RDP)
	int		opt_max_iter;		// maximum number of optimization iterations
} stereo_params;

// usage requirements:
// width	= 16*k
// cn		= 1
// ndisp	= 16*n

struct stereo_data;

// initializes disparity estimation object
// width		- rectified image width
// height		- rectified image height
// cn			- number of color channels (1 or 3)
// ndisp		- number of disparity levels
// wsize		- SAD window size
// lr_check		- left-right check flag
stereo_data *	stereo_init(int width, int height, int cn, stereo_params *params);

// performs diparity estimation on the specified stereo pair
// stereo		- allocated disparity estimation object
// im_l			- left rectified frame
// im_r			- right rectified frame
// disp			- left disparity map
// conf			- disparity confidence
int				stereo_process(stereo_data *stereo, unsigned char *im_l, unsigned char *im_r, int *disp, int *conf);

// releases disparity estimation object
// stereo		- allocated disparity estimation object
void			stereo_release(stereo_data *stereo);

/****************************************** Object Estimators *******************************************/

struct est_data;

typedef struct est_info {
	int nclasses;
	char **titles;
	int ref_w;
	int ref_h;
} est_info;

// load a trained estimator from specified file
est_data *	estimator_load(const char *path);

// release estimator
void		estimator_release(est_data *est);

// gets info about a loaded estimator
void		estimator_get_info(est_data *est, est_info *info);

// load an image for processing
// image	- image data (interlaced data order)
// step		- row stride
// width	- image width
// height	- image height
// cn		- number of bands
int			estimator_load_image(est_data *est, unsigned char *image, int step, int width, int height, int cn);

// perform classification or regression on the specified image region
// est		- estimator data
// x, y		- upper left corner of the window
// w		- window width
// h		- window height
// vals		- output values for regression
int			estimator_rect_class(est_data *est, int x, int y, int w, int h);
int			estimator_rect_regress(est_data *est, int x, int y, int w, int h, float *vals);

// perform classification or regression on the specified image region
// est		- estimator data
// x, y		- center of the window
// w		- window width
// h		- window height
// a		- window angle (degrees)
// vals		- output values for regression
int			estimator_center_class(est_data *est, int x, int y, int w, int h, int a);
int			estimator_center_regress(est_data *est, int x, int y, int w, int h, int a, float *vals);

/********************************************* Object detection functions **********************************************/

struct det_data;

typedef struct det_object {
	int			x;
	int			y;
	int			w;
	int			h;
	int			a;
	int			n;
} det_object;

typedef struct det_result {
	det_object *data;					// array of objects
	int			count;					// number of currently stored objects
	int			size;					// size of buffer allocated for objects
	int			width;					// image width
	int			height;					// image height
} det_result;

typedef struct det_params {
	bool	bDynamicTracking;
	bool	bVaryAngle;
	bool	bShowSubRects;
	int		wMin;
	int		wMax;
	double	fStepFine;
	double	fStepRough;
	int		iDilatations;
	int		iLayers;
} det_params;

det_data *		detector_create(int width, int height, int cn, est_data *est, det_params *params);
det_result *	detector_process(det_data *det, unsigned char *image, int step, det_result *result);
void			detector_release(det_data *det);

det_result *	detector_result_alloc(int count, det_result *result = 0);
void			detector_result_release(det_result *result);
