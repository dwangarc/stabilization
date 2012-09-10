/*
Construct History map, just add next image (mask in fact) and projection 
from the new image to the old one
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Take projection in the system where center of the image has coordinate (0, 0)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

class BackwardHM
{
public:
	BackwardHM();
	~BackwardHM();
	int load(unsigned char** image, int im_width, int im_height, int image_num, double** proj);
	unsigned char* get_history_map();
protected:
	int width;
	int height;
	int mapShift;

	unsigned char* HistoryMap;
	int	convert_image(unsigned char* img, double* proj, int width, int height);
};