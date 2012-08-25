#include "stabilization.h"
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define MATRIX_IDENTITY Mat(Matx33d(1.,0.,0.\
                                   ,0.,1.,0.\
                                   ,0.,0.,1.))
//
//Finding features
//
#define CORNERS_MAX_COUNT 200
//All corners with less quality coefficient are rejected
#define CORNERS_QUALITY 0.05
//Minimal distance allowed between corners
#define CORNERS_MIN_DISTANCE 1.0
//Don't apply any mask to search field
#define CORNERS_MASK noArray()
//Size of a block for computing derivative covariant matrix
#define CORNERS_BLOCK_SIZE 5
#define CORNERS_USE_HARRIS false
#define CORNERS_HARRIS_PARAM 0.04
//
//Refining features
//
//Half of the size of the search window used
#define CORNERS_WIN_SIZE Size(5,5)
//Half of the size of the central region where summation is not done
//due to possible singularities
#define CORNERS_DEAD_SIZE Size(-1,-1)
#define CORNERS_ITER_COUNT 30
#define CORNERS_ITER_EPS 0.03
//
//Lucas-Kanade
//
//Search window size at each pyramid level
#define LK_WIN_SIZE Size(15,15)
//How deep is constructed image pyramid
#define LK_MAX_LEVEL 2
#define LK_ITER_COUNT 20
#define LK_ITER_EPS 0.01
#define LK_FLAGS 0
#define LK_MIN_EIG_THRESHOLD 1e-3
//
//Transform estimation(refinement?)
//
#define OPTIMISTIC_K 3
#define OPTIMAL_DIST 2
#define MIN_DIST 0.5
#define MATRIX_MIN_TRANSFORM Mat(Matx33d( 2.0009, 0.0005, 0.09\
                                        , 0.0005, 2.0009, 0.09\
                                        , 0.0001, 0.0001, 2.00))
#define MATRIX_MAX_TRANSFORM Mat(Matx33d( 2.0, 0.7, 50\
                                        , 0.7, 2.0, 50\
                                        , 1.0, 1.0, 2.))
//Stabilization not required if all elements are respectively greater
//then in our transformation matrix
#define MATRIX_MAX_NORMAL_TRANSFORM Mat(Matx33d( 1.09, 0.10, 6\
                                               , 0.10, 1.09, 6\
                                               , 0.01, 0.01, 1))

class Frame {
public:
   Frame(int width, int height, void* image);
   cv::Mat img;
   cv::Mat grayImg;
   cv::Mat stabImg;
   std::vector<cv::Point2f> features;
   bool isEjected;
   int numOfStatic;
   cv::Mat transform;
};

Frame::Frame(int width, int height, void* image) {
   Mat mimg = Mat(height,width,CV_8UC3,image);
   img = mimg.clone();
   stabImg = img.clone();
   cvtColor(img,grayImg,CV_BGR2GRAY);
   isEjected = false;
   numOfStatic = 0;
   transform = MATRIX_IDENTITY;
}

void findFeatures(Frame* frame) {
   if(frame->isEjected || frame->features.size() < 0.8*CORNERS_MAX_COUNT)
      goodFeaturesToTrack( frame->grayImg, frame->features
                         , CORNERS_MAX_COUNT, CORNERS_QUALITY, CORNERS_MIN_DISTANCE
                         , CORNERS_MASK, CORNERS_BLOCK_SIZE, CORNERS_USE_HARRIS
                         , CORNERS_HARRIS_PARAM);
   if(frame->features.size() == 0) return;
   cornerSubPix(frame->grayImg, frame->features, CORNERS_WIN_SIZE, CORNERS_DEAD_SIZE
               , TermCriteria(TermCriteria::COUNT+TermCriteria::EPS
                             ,CORNERS_ITER_COUNT, CORNERS_ITER_EPS));
   // Draw circles around detected features.
   for(uint i = 0; i < frame->features.size(); i++) {
      circle(frame->img,frame->features[i],10,-1);
   }
}

void findTransform(Frame* lastFrame, Frame* frame) {
   frame->transform = MATRIX_IDENTITY;
   if(lastFrame->features.size() == 0) return;
   vector<float> errFeatures;
   vector<uchar> statusFeatures;
   // Optical flow by Lucas-Kanade
   calcOpticalFlowPyrLK( lastFrame->grayImg, frame->grayImg
                       , lastFrame->features, frame->features
                       , statusFeatures, errFeatures, LK_WIN_SIZE, LK_MAX_LEVEL
                       , TermCriteria( TermCriteria::COUNT+TermCriteria::EPS
                                     , LK_ITER_COUNT, LK_ITER_EPS)
                       , LK_FLAGS, LK_MIN_EIG_THRESHOLD);
   if(lastFrame->features.size() != frame->features.size()) return;
   for(uint i = 0; i < statusFeatures.size(); i++) {
      if(!statusFeatures[i]) {
         frame->features.erase(frame->features.begin()+i);
         lastFrame->features.erase(lastFrame->features.begin()+i);
         statusFeatures.erase(statusFeatures.begin()+i);
         i--;
      }
   }
   if(statusFeatures.size() < 4) return;
   // For some reason arguments should be reversed to what documentation says.
   frame->transform = findHomography(frame->features, lastFrame->features, CV_RANSAC);
}

//Why is it called such?
double calcDistanceTo(const Mat& mat, const Mat& src_mat) {
   double dist, max_dist = 0;
   for(int i = 0; i < 3; i++)
      for(int j = 0; j < 3; j++) {
         //Should there be another abs?
         dist = abs(mat.at<double>(i,j)) / src_mat.at<double>(i,j);
         if(dist > max_dist) max_dist = dist;
      }
   return max_dist;
}

//What numOfStatic is supposed to mean?
void refineTransform(Frame* lastFrame, Frame* frame) {
   Mat orig_stab_tr = lastFrame->transform;
   if(calcDistanceTo(frame->transform, MATRIX_MAX_TRANSFORM) > 1) {
      frame->isEjected = true;
      frame->transform = MATRIX_IDENTITY;
      return;
   } else if(calcDistanceTo(frame->transform, MATRIX_MIN_TRANSFORM) < 1)
      frame->numOfStatic = lastFrame->numOfStatic + 1;
   else
      orig_stab_tr = frame->transform * orig_stab_tr;

   if(calcDistanceTo(orig_stab_tr, MATRIX_MIN_TRANSFORM) < 1) {
      frame->transform = MATRIX_IDENTITY;
      return;
   }
   
   double orig_stab_dist = calcDistanceTo( orig_stab_tr - MATRIX_IDENTITY
                                         , MATRIX_MAX_NORMAL_TRANSFORM * MIN_DIST);

   if(orig_stab_dist <= 1) {
      if(frame->numOfStatic > OPTIMISTIC_K + 2)
         frame->transform = 0.8 * orig_stab_tr + 0.2 * MATRIX_IDENTITY;
      else frame->transform = orig_stab_tr;
   } else {
      if(lastFrame->numOfStatic > OPTIMISTIC_K) {
         double func_val = pow(log(orig_stab_dist) + 1, -1./OPTIMAL_DIST);
         frame->transform = func_val * orig_stab_tr + (1-func_val) * MATRIX_IDENTITY;
      } else {
         frame->numOfStatic = lastFrame->numOfStatic + 1;
         frame->transform = 0.9 * orig_stab_tr + 0.1 * MATRIX_IDENTITY;
      }
   }
}

void stabilize(Frame* lastFrame, Frame* frame) {
   findFeatures(lastFrame);
   findTransform(lastFrame, frame);
   refineTransform(lastFrame, frame);
   // Apply transformation
   warpPerspective(frame->img, frame->stabImg, frame->transform, frame->img.size());
}

class StabilizerData {
public:
   Frame* frame;
   Frame* prevFrame;
   int width;
   int height;
};

Stabilizer::Stabilizer(int width, int height) {
   data = new StabilizerData(); 
   data->width = width;
   data->height = height;
}

Stabilizer::~Stabilizer() {
   if(data->frame) delete data->frame;
   if(data->prevFrame) delete data->prevFrame;
   delete data;
}

int Stabilizer::addFrame(void* image, int width, int height) {
   if(width != -1) {
      if(data->width == -1) data->width = width;
      else if(data->width != width) return -1;
   } else if(data->width == -1) return -1;
   if(height != -1) {
      if(data->height == -1) data->height = height;
      else if(data->height != height) return -1;
   } else if(data->height == -1) return -1;
   Frame* frame = new Frame(data->width, data->height, image);
   if(!data->frame) data->frame = frame;
   else {
      delete data->prevFrame;
      data->prevFrame = data->frame;
      data->frame = frame;
      stabilize(data->prevFrame, data->frame);
   }
   return 0;
}

void* Stabilizer::getStabilizedImage() { return this->data->frame->stabImg.ptr(); }

void* Stabilizer::getOriginalImage() { return this->data->frame->img.ptr(); }
