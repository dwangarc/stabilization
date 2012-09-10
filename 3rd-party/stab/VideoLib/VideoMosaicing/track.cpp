#include "track.h"


void* CreateTrack(int width, int height, int method)
{
	return ml_track_create(width, height);
}
int InitTrack(void *data, unsigned char *im, int step, track_target *target, int method)
{
	return ml_track_init((ml_track_data*)data, 
											im, step, target);
	return 0;
}
int Track(void* data, unsigned char* im, int step, track_target* target, int method)
{
	return ml_track_track((ml_track_data*)data, im, step, target);
	return 0;
}
void EndTrackData(void** data, int method)
{
	ml_track_release((ml_track_data*)*data);
	*data = 0;
}