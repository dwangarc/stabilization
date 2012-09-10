/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Give projection in the system where center of the image has coordinate (0, 0)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

class inLine 
{
public:
	inLine();
	~inLine();

	int load(unsigned char* image, unsigned char* imageRGB, double* projection, int im_width, int im_height);
	unsigned char** get_diff();
	double** get_proj();
	unsigned char** get_im();
	unsigned char** get_imRGB();
	int change_lineNumber(int new_lineNumber);
	int change_course(int new_course);
	int change_treshold(int new_treshold);

	unsigned char** im;


protected:
	int lineNumber;
	unsigned char** imRGB;
	unsigned char** diff;
	double**		proj;
	int width;
	int height;

	int course;
	int treshold;
	int gauss_depth;
	
	int gauss_filter(unsigned char* im, int im_width, int im_height);//gauss filter	
	int gauss_sweep_one(unsigned char** dxx, int im_width, int* gxx);
	int gauss_sweep_two(int* gxx, int im_width, int* Gxx);
	int gauss_sweep_three(int* Gxx, int im_width, int* s);
	int	gauss_sweep_four(int * s, int im_width, int im_height, unsigned char* im);
	int shift_queue();

	int calculate_diff();
	int calculate_projection(double* projection);

	int clear_data();
	double maxDiff;
};
