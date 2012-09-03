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
//Transform refinement
//
#define MAX_ERROR Mat(Matx34d( 1.0, 0.5, 1., 1\
                             , 0.5, 1.0, 1., 1\
                             , 1.0, 1.0, 1., 1))
#define MAX_DIST 200

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
   cv::Mat pose;
};

Frame::Frame(int width, int height, void* image) {
   Mat mimg = Mat(height,width,CV_8UC3,image);
   img = mimg.clone();
   stabImg = img.clone();
   cvtColor(img,grayImg,CV_BGR2GRAY);
   isEjected = false;
   numOfStatic = 0;
   transform = Mat::eye(3,3,CV_64F);
   pose = Mat::eye(3,4,CV_64F);
}

class PID {
public:
   PID(double kp, double ki, double kd);
   double kp;
   double ki;
   double kd;
   void fix(const Mat& src, Mat& dest);
   Mat totalError;
   Mat lastError;
};

PID::PID(double kp, double ki, double kd): kp(kp), ki(ki), kd(kd) {
   totalError = Mat::zeros(3,4,CV_64F);
   lastError = Mat::zeros(3,4,CV_64F);
}

void PID::fix(const Mat& src, Mat& dest) {
   Mat err = src-dest;
   cout << "error: " << err << endl;
   totalError += err;
   Mat adj = Mat::zeros(3,4,CV_64F);
   for(int i=0;i<3;i++) for(int j=0;j<4;j++) {
      //adj.at<double>(i,j) = (atan((abs(err.at<double>(i,j))-MAX_ERROR.at<double>(i,j))*100)+1.5)/3.2;
      //adj.at<double>(i,j) = pow(abs(err.at<double>(i,j))/MAX_ERROR.at<double>(i,j),5);
      adj.at<double>(i,j) = kp*err.at<double>(i,j);
      //if(adj.at<double>(i,j) > 1) adj.at<double>(i,j) = 1;
      //if(adj.at<double>(i,j) < 0) adj.at<double>(i,j) = 0;
      //if(err.at<double>(i,j) < 0) adj.at<double>(i,j)*=-1;
   }
   adj += totalError*ki + (lastError-err)*kd;
   cout << "adj: " << adj << endl;
   lastError = err;
   dest = dest+adj;
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
}

void findTransform(Frame* lastFrame, Frame* frame) {
   frame->transform = Mat::eye(3,3,CV_64F);
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

void setupKalman(KalmanFilter* kf) {
   kf->init(24,12,0,CV_64F);
   kf->transitionMatrix = Mat::eye(24,24,CV_64F);
   for(int i=0;i<12;i++) { kf->transitionMatrix.at<double>(i,i+12)=1; }
//   cout << "Transition matrix: " << kf->transitionMatrix << endl;
   setIdentity(kf->measurementMatrix);
   kf->statePost = (Mat_<double>(24,1) << 1,0,0,0
                                         ,0,1,0,0
                                         ,0,0,1,0
                                         ,0,0,0,0
                                         ,0,0,0,0
                                         ,0,0,0,0);
   kf->processNoiseCov = Mat::eye(24,24,CV_64F)*1e-4;
   //cout << "Process noise covariance matrix: " << kf->processNoiseCov << endl;
   kf->measurementNoiseCov = Mat::eye(12,12,CV_64F);
   //cout << "Measurement noise covariance matrix" << kf->measurementNoiseCov << endl;
}

void cameraPoseFromHomography(const Mat& H, const Mat& A, const Mat& invA, const Mat& lastPose, Mat& pose) {
   pose = invA*H*A*lastPose;
}

void homographyFromCameraPose(const Mat& pose, const Mat& A, const Mat& invA, const Mat& lastPose, Mat& H) {
   H = A*pose*lastPose.inv(DECOMP_SVD)*invA;
}

void cameraMatrixFromParams(int width, int height, double focal, Mat& A, Mat& invA) {
   double invF = 1./focal;
   double cx = width/2.;
   double cy = height/2.;
   invA = (Mat_<double>(3,3) << invF, 0, -cx*invF
                              , 0, invF, -cy*invF
                              , 0, 0, 1);
   A = (Mat_<double>(3,3) << focal,0,cx
                            ,0,focal,cy
                            ,0,0,1);
}

//kalmanPose is vector now
void normalizePose(Mat& kalmanPose, Mat& pose) {
   Mat poseKF = kalmanPose.clone().reshape(0,6);
   poseKF.resize(3);
   poseKF = poseKF.inv(DECOMP_SVD);
   for(int i=0;i<12;i++) kalmanPose.at<double>(i) = 0;
   kalmanPose.at<double>(0) = 1;
   kalmanPose.at<double>(5) = 1;
   kalmanPose.at<double>(10) = 1;
   pose = pose * poseKF * Mat::eye(3,4,CV_64F);
}

//If transformation is too agressive, make it Identity
void snapHomography(double width, double height, Mat& homography, Mat& pose, Mat& kalmanPose) {
   double dist = 0;
   Mat tl = (Mat_<double>(3,1) << 0,0,1);
   Mat tr = (Mat_<double>(3,1) << width,0,1);
   Mat bl = (Mat_<double>(3,1) << 0,height,1);
   Mat br = (Mat_<double>(3,1) << width,height,1);
   dist += norm(tl, homography * tl);
   dist += norm(tr, homography * tr);
   dist += norm(bl, homography * bl);
   dist += norm(br, homography * br);
   cout << "Dist is: " << dist << endl;
   if(dist > MAX_DIST) {
      cout << "tl: " << tl << "-> " << homography*tl << endl;
      cout << "tl: " << tr << "-> " << homography*tr << endl;
      cout << "tl: " << bl << "-> " << homography*bl << endl;
      cout << "tl: " << br << "-> " << homography*br << endl;
      homography = Mat::eye(3,3,CV_64F);
      pose = Mat::eye(3,4,CV_64F);
      for(int i=12;i<24;i++) kalmanPose.at<double>(i) = 0;
   }
}

void simplifyTransform(double width, double height, Mat& homography) {
   Mat tl = (Mat_<double>(3,1) << 0,0,1);
   Mat tr = (Mat_<double>(3,1) << width,0,1);
   Mat bl = (Mat_<double>(3,1) << 0,height,1);
   Mat br = (Mat_<double>(3,1) << width,height,1);
   Mat v[] = {tl,tr,bl,br};
   int i;
   //cout << "Homographed points: " << endl;
   for(i=0;i<4;i++) {
      v[i] = homography * v[i];
      v[i] = v[i] / v[i].at<double>(2,0);
      cout << i << ": " << v[i] << endl;
   }
   Mat pivot = (v[0]+v[1]+v[2]+v[3])/4;
   //cout << "Pivot is: " << pivot << endl;
   double avgDist = 0;
   //cout << "Pivot vectors are: " << endl;
   for(i=0;i<4;i++) {
      v[i] = v[i]-pivot;
      cout << i << ": " << v[i] << endl;
      avgDist += norm(v[i]);
   }
   avgDist/=4;
   //cout << "Average distance is: " << avgDist << endl;
   //cout << "New pivot vectors are: " << endl;
   for(i=0;i<4;i++) {
      v[i] = pivot + v[i]/norm(v[i])*avgDist;
      cout << i << ": " << v[i] << endl;
   }
   Point2f origPoints[] = {Point2f(0,0), Point2f(width,0), Point(0,height), Point(width,height)};
   Point2f newPoints[4];
   //cout << "Mapping: " << endl;
   for(i=0;i<4;i++) {
      newPoints[i] = Point2f((float)v[i].at<double>(0,0),(float)v[i].at<double>(1,0));
      cout << origPoints[i] << " -> " << newPoints[i] << endl;
   }
   homography = getAffineTransform(origPoints,newPoints);
   homography.resize(3,0);
   homography.at<double>(2,2) = 1;
}

void refineTransform(KalmanFilter* kalman, PID* pid, Frame* lastFrame, Frame* frame) {
   //cout << endl << "Before: " << frame->transform << endl;
   Mat A, invA;
   cameraMatrixFromParams(frame->img.cols, frame->img.rows, 30, A, invA);
   //cout << "Camera matrix: " << A << endl;
   cameraPoseFromHomography(frame->transform, A, invA, lastFrame->pose, frame->pose);
   //cout << "Last pose: " << lastFrame->pose << endl << "Pose: " << frame->pose << endl;
   Mat pose = frame->pose.clone();
   kalman->predict();
   Mat kalmanPose = kalman->correct(pose.reshape(0,12));
   normalizePose(kalmanPose, pose);
   //cout << "Normalized pose: " << pose << endl;
   kalman->statePost = kalmanPose.clone();
   kalmanPose = kalmanPose.reshape(0,6);
   kalmanPose.resize(3);
   frame->pose = pose.clone();
   homographyFromCameraPose(pose,A,invA,kalmanPose,frame->transform);
   //cout << "After pose estimation: " << frame->transform << endl;
   simplifyTransform(frame->img.cols, frame->img.rows, frame->transform);
   //cout << "After simplification: " << frame->transform << endl;
   snapHomography(frame->img.cols, frame->img.rows, frame->transform, frame->pose, kalman->statePost);
   //cout << "After snapping: " << frame->transform << endl;
}

void stabilize(KalmanFilter* kalman, PID* pid, Frame* lastFrame, Frame* frame) {
   findFeatures(lastFrame);
   findTransform(lastFrame, frame);
   refineTransform(kalman, pid, lastFrame, frame);
   // Apply transformation
   Mat mask = Mat::ones(frame->img.rows, frame->img.cols, CV_64F);
   Mat warpedMask = Mat::zeros(frame->img.rows, frame->img.cols, CV_64F);
   frame->stabImg = lastFrame->stabImg.clone();
   warpPerspective(frame->img, frame->stabImg, frame->transform, frame->img.size());
   warpPerspective(mask,warpedMask,frame->transform,frame->img.size());
   for(int i=0;i<mask.rows;i++) for(int j=0;j<mask.cols;j++) {
      frame->stabImg.at<Vec3b>(i,j) += (1-warpedMask.at<double>(i,j))*lastFrame->stabImg.at<Vec3b>(i,j);
   }
   // Draw circles around detected features.
   //for(uint i = 0; i < frame->features.size(); i++) {
   //   circle(frame->img,frame->features[i],10,-1);
   //}
}

class StabilizerData {
public:
   Frame* frame;
   Frame* prevFrame;
   int width;
   int height;
   KalmanFilter* kalman;
   PID* pid;
};

Stabilizer::Stabilizer(int width, int height) {
   data = new StabilizerData();
   data->width = width;
   data->height = height;
   data->kalman = new KalmanFilter(32,16,0,CV_64F);
   double kp = 0.3;
   double ki = 0.0;
   double kd = 0.2;
   data->pid = new PID(kp,ki,kd);
   setupKalman(data->kalman);
}

Stabilizer::~Stabilizer() {
   if(data->frame) delete data->frame;
   if(data->prevFrame) delete data->prevFrame;
   if(data->kalman) delete data->kalman;
   if(data->pid) delete data->pid;
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
      stabilize(data->kalman, data->pid, data->prevFrame, data->frame);
   }
   return 0;
}

void* Stabilizer::getStabilizedImage() { return this->data->frame->stabImg.ptr(); }

void* Stabilizer::getOriginalImage() { return this->data->frame->img.ptr(); }
