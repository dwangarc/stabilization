/*
Construct History map, just add next image (mask in fact) and projection 
from the new image to the old one
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Take projection in the system where center of the image has coordinate (0, 0)
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

class ForwardHM
{
public:
	ForwardHM();
	~ForwardHM();
	int load(unsigned char* image, int im_width, int im_height, double* im_proj);
	unsigned char* get_history_map();

protected:
	int width;
	int height;
	int mapShift;

	unsigned char* HistoryMap;
};