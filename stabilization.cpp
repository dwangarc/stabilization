#include "stabilization.h"
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define MATRIX_IDENTITY Mat(Matx33d(1.,0.,0.\
                                   ,0.,1.,0.\
                                   ,0.,0.,1.))
//Finding features
#define CORNERS_MAX_COUNT 200
#define CORNERS_QUALITY 0.05
#define CORNERS_MIN_DISTANCE 1.0
#define CORNERS_MASK noArray()
#define CORNERS_BLOCK_SIZE 5
#define CORNERS_USE_HARRIS false
#define CORNERS_HARRIS_PARAM 0.04
//Refining features
#define CORNERS_WIN_SIZE Size(5,5)
#define CORNERS_DEAD_SIZE Size(-1,-1)
#define CORNERS_ITER_COUNT 30
#define CORNERS_ITER_EPS 0.03
//Lucas-Kanade
#define LK_WIN_SIZE Size(15,15)
#define LK_MAX_LEVEL 2
#define LK_ITER_COUNT 20
#define LK_ITER_EPS 0.01
#define LK_FLAGS 0
#define LK_MIN_EIG_THRESHOLD 1e-3
//Transform estimation
#define OPTIMISTIC_K 3
#define OPTIMAL_DIST 2
#define MIN_DIST 0.5
#define MATRIX_MIN_TRANSFORM Mat(Matx33d( 1.0009, 0.0005, 0.09\
                                        , 0.0005, 1.0009, 0.09\
                                        , 0.0001, 0.0001, 1.00))
#define MATRIX_MAX_TRANSFORM Mat(Matx33d( 1.0, 0.7, 50\
                                        , 0.7, 1.0, 50\
                                        , 1.0, 1.0, 1.))
//Stabilization not required if all elements are respectively greater
//then in our transformation matrix
#define MATRIX_MAX_NORMAL_TRANSFORM Mat(Matx33d( 1.09, 0.10, 6\
                                               , 0.10, 1.09, 6\
                                               , 0.01, 0.01, 1))

class FrameData {
public:
   cv::Mat img;
   cv::Mat grayImg;
   cv::Mat stabImg;
   std::vector<cv::Point2f> features;
   bool isEjected;
   int numOfStatic;
   cv::Mat transformToPrev;
   cv::Mat transformToOrig;
};

Frame::Frame(int width, int height, void* image) {
   this->data = new FrameData();
   Mat img = Mat(height,width,CV_8UC3,image);
   this->data->img = img.clone();
   //cvtColor(this->data->img,this->data->img,CV_RGB2BGR);
   this->data->stabImg = this->data->img.clone();
   cvtColor(this->data->img,this->data->grayImg,CV_BGR2GRAY);
   this->data->isEjected = false;
   this->data->numOfStatic = 0;
   this->data->transformToPrev = MATRIX_IDENTITY;
   this->data->transformToOrig = MATRIX_IDENTITY;
}

Frame::~Frame() { delete data; }

void* Frame::getStabilizedImage() {
   if(!this->data->stabImg.isContinuous()) cout << "Not continuous stab" << endl;
   return this->data->stabImg.ptr();
   //Mat res = this->data->stabImg.clone();
   //cvtColor(res,res,CV_BGR2RGB);
   //if(!res.isContinuous()) cout << "Not continuous stab" << endl;
   //return res.ptr();
}

void* Frame::getOriginalImage() {
   if(!this->data->img.isContinuous()) cout << "Not continuous orig" << endl;
   return this->data->img.ptr();
   //Mat res = this->data->img.clone();
   //cvtColor(res,res,CV_BGR2RGB);
   //if(!res.isContinuous()) cout << "Not continuous orig" << endl;
   //return res.ptr();
}

void findFeatures(FrameData* frame) {
   if(frame->isEjected || frame->features.size() < 0.8*CORNERS_MAX_COUNT)
      goodFeaturesToTrack( frame->grayImg, frame->features
                         , CORNERS_MAX_COUNT, CORNERS_QUALITY, CORNERS_MIN_DISTANCE
                         , CORNERS_MASK, CORNERS_BLOCK_SIZE, CORNERS_USE_HARRIS
                         , CORNERS_HARRIS_PARAM);
   cornerSubPix(frame->grayImg, frame->features, CORNERS_WIN_SIZE, CORNERS_DEAD_SIZE
               , TermCriteria(TermCriteria::COUNT+TermCriteria::EPS
                             ,CORNERS_ITER_COUNT, CORNERS_ITER_EPS));
   //for(int i = 0; i < frame->features.size(); i++) {
   //   circle(frame->img,frame->features[i],10,-1);
   //}
}

Mat findTransform(FrameData* last_frame, FrameData* frame, vector<uchar> status) {
   if(last_frame->features.size() != frame->features.size())
      return MATRIX_IDENTITY;
   for(int i = 0; i < status.size(); i++) {
      if(!status[i]) {
         frame->features.erase(frame->features.begin()+i);
         last_frame->features.erase(last_frame->features.begin()+i);
         status.erase(status.begin()+i);
         i--;
      }
   }
   if(status.size() < 4) { return MATRIX_IDENTITY; }
   //return findHomography(last_frame.features, frame.features, CV_RANSAC);
   //For some reason the inverse matrix is actually required
   return findHomography(frame->features, last_frame->features, CV_RANSAC);
}

bool checkEjection(Mat transform) {
   for(int i = 0; i < 3; i++)
      for(int j = 0; j < 3; j++)
         if(abs(transform.at<double>(i,j)) > MATRIX_MAX_TRANSFORM.at<double>(i,j)) {
            cout << "Ejection" << endl;
            return true;
         }
   return false;
}

//Why is it called such?
double calcDistanceTo(const Mat& mat, const Mat& src_mat) {
   double dist, max_dist;
   for(int i = 0; i < 3; i++)
      for(int j = 0; j < 3; j++) {
         //Should there be another abs?
         dist = abs(mat.at<double>(i,j)) / src_mat.at<double>(i,j);
         if(dist > max_dist) max_dist = dist;
      }
   return max_dist;
}

//What numOfStatic is supposed to mean?
void estimateTransform(FrameData* frame, FrameData* lastFrame, Mat& transform) {
   Mat transf_vals = transform - MATRIX_IDENTITY;
   if(checkEjection(transf_vals)) { frame->isEjected = true; transform = MATRIX_IDENTITY; return; }
   if(calcDistanceTo(transf_vals, MATRIX_MIN_TRANSFORM) < 1)
      frame->numOfStatic = lastFrame->numOfStatic + 1;
   else
      frame->transformToPrev = transform;

   double orig_dist = calcDistanceTo(transf_vals, MATRIX_MAX_NORMAL_TRANSFORM);
   Mat orig_stab_tr = frame->transformToPrev * lastFrame->transformToOrig;
   Mat stab_transf_vals = orig_stab_tr - MATRIX_IDENTITY;
   if(calcDistanceTo(stab_transf_vals, MATRIX_MIN_TRANSFORM) < 1) { frame->isEjected = true; transform = MATRIX_IDENTITY; return; }
   
   double orig_stab_dist = calcDistanceTo( stab_transf_vals
                                         , MATRIX_MAX_NORMAL_TRANSFORM * MIN_DIST);
   double func_val = 1;
   if(orig_stab_dist > 1)
      func_val = 1. / pow(log(orig_stab_dist)+1, 1. / OPTIMAL_DIST);

   Mat opt_stab_tr = stab_transf_vals * func_val + MATRIX_IDENTITY;
   Mat stab_stab_tr = stab_transf_vals * (1 - func_val) + MATRIX_IDENTITY;

   //cout << "original dist: " << orig_dist << endl;
   //cout << "original -> stab dist: " << orig_stab_dist << endl;
   //cout << "numOfStatic on start: " << frame.numOfStatic << endl;

   if(orig_stab_dist < 1) {
      if(frame->numOfStatic - 2 > OPTIMISTIC_K) {
         //cout << "transform is stab_transf_vals" << endl;
         transform = stab_transf_vals * 0.8 + MATRIX_IDENTITY;
      }
      else {
         //cout << "transform is orig_stab_tr" << endl;
         transform = orig_stab_tr;
      }
      frame->transformToOrig = transform;
   } else {
      if(lastFrame->numOfStatic > OPTIMISTIC_K) {
         frame->transformToOrig = opt_stab_tr;
         frame->transformToPrev = stab_stab_tr;
         //cout << "transform is opt_stab_tr" << endl;
         transform = opt_stab_tr;
      } else {
         frame->numOfStatic = lastFrame->numOfStatic + 1;
         //cout << "transform is stab_tranf_vals(2)" << endl;
         transform = stab_transf_vals * 0.9 + MATRIX_IDENTITY;
         frame->transformToOrig = transform;
      }
   }
   //cout << "numOfStatic: " << frame.numOfStatic << endl;
   return;
}

void DECL_EXPORT stabilize(Frame* lastFrame, Frame* frame) {
   //cout << "Commence stabilization" << endl;
   FrameData* frameData = frame->data;
   FrameData* lastFrameData = lastFrame->data;
   findFeatures(lastFrameData);
   //cout << "Found features" << endl;
   vector<float> errFeatures;
   vector<uchar> statusFeatures;
   calcOpticalFlowPyrLK( lastFrameData->grayImg, frameData->grayImg
                       , lastFrameData->features, frameData->features
                       , statusFeatures, errFeatures, LK_WIN_SIZE, LK_MAX_LEVEL
                       , TermCriteria( TermCriteria::COUNT+TermCriteria::EPS
                                     , LK_ITER_COUNT, LK_ITER_EPS)
                       , LK_FLAGS, LK_MIN_EIG_THRESHOLD);
   Mat transform = findTransform(lastFrameData, frameData, statusFeatures);
   estimateTransform(frameData, lastFrameData, transform);
   warpPerspective(frameData->img, frameData->stabImg, transform, frameData->img.size());
}
