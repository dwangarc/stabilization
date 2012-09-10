class HistRatio {
public:
	HistRatio();
	~HistRatio();

	int init_track(unsigned char* image, int im_width, int im_height, 
		int in_x, int in_y, int in_w, int in_h);
	int hist_ratio(unsigned char* image, int im_width, int im_height, unsigned char* mask);
	int load_data(int in_x, int in_y, int in_width, int in_height);
	int read_data(int &out_x, int &out_y, int &out_width, int &out_height);
	int non_zero_data();
	int reset_initialized();

protected:
	int x;
	int y;
	int w;
	int h;

	int Initialized;

	int width;
	int height;
	
	int BinNum;
	int TotalBins;
	int BorderShift;
	int Range;
	int L;
	int Treshold;

	double* HistInit;
	double* HistModel;
	double* HistDivide;

	double* ImBackProj;

	double Alpha;

	void hist_divide(double* hist1, double* hist2, double* hist_res);
	void histogram_normalize(double* hist);
	void calc_foreground(unsigned char* image);
	double* calc_bp(unsigned char* image, double* hist);
	void count_hist(unsigned char* image);
	void shift();

	void upgrade_hist(unsigned char* image, unsigned char* mask);
};