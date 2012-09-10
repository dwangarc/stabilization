#pragma once

typedef struct track_target {
	int x;
	int y;
	int w;
	int h;
} track_target;

struct ml_track_data;

ml_track_data*	ml_track_create(int width, int height);
int				ml_track_init(ml_track_data *data, unsigned char* im, int step, track_target *target);
double			ml_track_track(ml_track_data *data, unsigned char* &im, int step, 
							   track_target *target);
void			ml_track_release(ml_track_data *data);