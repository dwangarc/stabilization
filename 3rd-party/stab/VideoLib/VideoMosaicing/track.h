#pragma once

typedef struct track_target {
	int x;
	int y;
	int w;
	int h;
} track_target;

struct ms_track_data;
struct kms_track_data;
struct bs_track_data;
struct df_track_data;
struct apf_track_data;
struct fgk_track_data;
struct hr_track_data;
struct tm_track_data;
struct pf_track_data;
struct vr_track_data;
struct avr_track_data;
struct pd_track_data;
struct apd_track_data;
struct ml_track_data;

#include "mean_shift.h"
#include "kalman_mean_shift.h"
#include "blob_scale.h"
#include "discriminative_features.h"
#include "annealled_particle_filter.h"
#include "fast_global_kernel.h"
#include "histogram_ratio.h"
#include "template_match.h"
#include "particle_filter.h"
#include "variance_ratio.h"
#include "adaptive_variance_ratio.h"
#include "peak_difference.h"
#include "adaptive_peak_difference.h"
#include "moving_localization.h"


#define	MEAN_SHIFT					1
#define KALMAN_MEAN_SHIFT			2
#define BLOB_SCALE					3
#define DISCRIMINATIVE_FEATURES		4
#define ANNEALLED_PARTICLE_FILTER	5
#define FAST_GLOBAL_KERNEL			6
#define HISTOGRAM_RATIO				7
#define TEMPLATE_MATCH				8
#define PARTICLE_FILTER				9
#define VARIANCE_RATIO				10
#define ADAPTIVE_VARIANCE_RATIO		11
#define PEAK_DIFFERENCE				12
#define ADAPTIVE_PEAK_DIFFERENCE	13
#define MOVING_LOCALIZATION			14


void* CreateTrack(int width, int height, int method);
int Track(void* data, unsigned char* im, int step, track_target* target, int method);
int InitTrack(void *data, unsigned char *im, int step, track_target *target, int method);
void EndTrackData(void** data, int method);

