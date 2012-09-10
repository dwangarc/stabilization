class MeanShift{
public:
	MeanShift();
	~MeanShift();

	int init_track(unsigned char* image, int im_width, int im_height, 
		int in_x, int in_y, int in_w, int in_h);
	int mean_shift(unsigned char* image, int im_width, int im_height, unsigned char* mask);
	int load_data(int in_x, int in_y, int in_width, int in_height);
	int read_data(int &out_x, int &out_y, int &out_width, int &out_height);
	int non_zero_data();
	int reset_initialized();
	int change_mode(int new_mode);

	int BinNum;
	int TotalBins;

	double* HistInit;
protected:
	int mode;//1 - simple mean-shift; 2 - mask mean shift; 3 - simple + mask mean-shift
	int upgrate_mode;
	double alpha;
	
	int x;
	int y;
	int w;
	int h;

	int Initialized;

	int width;
	int height;
	
	double Alpha;

	int extract_histogram(unsigned char *im, int w, int h, int x0, int y0, 
							int hx, int hy, double *hist);
	int shift(unsigned char *im, int step, int width, int height, int x0, int y0, 
		  int hx, int hy, double *w, int &x1, int &y1, unsigned char* mask);

	int upgrate_hist(unsigned char* im, unsigned char* mask);
};