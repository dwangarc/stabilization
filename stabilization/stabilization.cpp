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
#define MIN_SIN 0.01
#define MAX_SIN 0.1
#define MIN_SCALE 1.01
#define MAX_SCALE 1.5
#define MIN_TRANS 0.1
#define MAX_TRANS 10

#define OPTIMISTIC_K 1
#define OPTIMAL_DIST 2
#define MATRIX_MIN_TRANSFORM Mat(Matx33d( 1.10, 1e-2, 0.1\
                                        , 1e-2, 1.10, 0.1\
                                        , 1e-5, 1e-5, 1.1))
#define MATRIX_MAX_TRANSFORM Mat(Matx33d( 1.5, 1.0, 10\
                                        , 1.0, 1.5, 10\
                                        , 1.0, 1.0, 1.))
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
   cv::Mat cameraTransform;
};

Frame::Frame(int width, int height, void* image) {
   Mat mimg = Mat(height,width,CV_8UC3,image);
   img = mimg.clone();
   stabImg = img.clone();
   cvtColor(img,grayImg,CV_BGR2GRAY);
   isEjected = false;
   numOfStatic = 0;
   transform = Mat::eye(3,3,CV_64F);
   cameraTransform = Mat::eye(3,3,CV_64F);
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
void refineTransform1(Frame* lastFrame, Frame* frame) {
   //a01, a10 must be less then 1 and bigger than 0.1
   double stheta = (frame->transform.at<double>(1,0)-frame->transform.at<double>(0,1))/2;
   double scale = (frame->transform.at<double>(0,0)+frame->transform.at<double>(1,1))/sqrt(1-stheta*stheta)/2;
   double tx = frame->transform.at<double>(0,2);
   double ty = frame->transform.at<double>(1,2);
   double skewX = frame->transform.at<double>(2,0);
   double skewY = frame->transform.at<double>(2,1);
   if(stheta > MAX_SIN || stheta < MIN_SIN) stheta = 0;
   if( scale < 1 && (1/scale > MAX_SCALE || 1/scale < MIN_SCALE)
     || scale >= 1 && (scale > MAX_SCALE || scale < MIN_SCALE)) scale = 1;
   if(tx > MAX_TRANS || tx < MIN_TRANS) tx = 0;
   if(ty > MAX_TRANS || ty < MIN_TRANS) ty = 0;;
   double ctheta = sqrt(1-stheta*stheta);
   double m[3][3] = {{scale*ctheta,-stheta,tx},{stheta,scale*ctheta,ty},{0,0,1}};
   frame->transform = Mat(3,3,CV_64F, m);
   cout << "Want transform: " << frame->transform << endl;
   //cout << endl;
   //cout << "Want sin(theta)=" << stheta << "\nscale=" << scale << "\nt=" << tx << "," << ty << "\nskew=" << skewX << "," << skewY << endl;
   Mat orig_stab_tr = lastFrame->transform;
   //Mat orig_stab_tr = MATRIX_IDENTITY;
   if(calcDistanceTo(frame->transform, MATRIX_MAX_TRANSFORM) > 1) {
      //cout << "Transformation is too strong. Assuming identity" << endl;
      frame->isEjected = true;
      frame->transform = MATRIX_IDENTITY;
      return;
   } else if(calcDistanceTo(frame->transform, MATRIX_MIN_TRANSFORM) <= 1) {
      //cout << "Transformation is too week. Using previous" << endl; 
      frame->numOfStatic = lastFrame->numOfStatic + 1;
   } else {
      //cout << "Transformation is OK. Using composition of current and previous" << endl;
      orig_stab_tr = frame->transform * orig_stab_tr;
   }

   if(calcDistanceTo(orig_stab_tr, MATRIX_MIN_TRANSFORM) < 1) {
      //cout << "New transformation is too week. Assuming identity." << endl;
      frame->transform = MATRIX_IDENTITY;
      return;
   }
   
   double orig_stab_dist = calcDistanceTo(orig_stab_tr, MATRIX_MAX_NORMAL_TRANSFORM);

   double weight;
   if(orig_stab_dist <= 1) {
      //cout << "New transformation has not reached something" << endl;
      if(frame->numOfStatic > OPTIMISTIC_K + 2) {
         //cout << "New transformation has been applied to too many frames. Damping." << endl;
         weight = 0.8;
      } else {
         //cout << "New transformation is the best choice" << endl;
         weight = 0.9;
      }
   } else {
      //cout << "New transformation has reached something" << endl;
      if(lastFrame->numOfStatic > OPTIMISTIC_K) {
         //cout << "lastFrame->numOfStatic > OPTIMISTIC_K. Applying smart damping" << endl;
         //double func_val = pow(log(orig_stab_dist) + 1, -1./OPTIMAL_DIST);
         weight = 0.8;
      } else {
         //cout << "lastFrame->numOfStatic <= OPTIMISITIC_K. Still applying damping. And assuming frame static." << endl;
         frame->numOfStatic = lastFrame->numOfStatic + 1;
         weight = 0.9;
      }
   }
   //frame->transform = weight * orig_stab_tr + (1-weight) * MATRIX_IDENTITY;
   //cout << "Will apply transformation: " << frame->transform << endl;
}

void cameraTransformFromHomography(const Mat& H, const Mat& A, const Mat& invA, Mat& transf) {
   transf = invA*H*A;
}

void homographyFromCameraTransform(const Mat& transf, const Mat& A, const Mat& invA, Mat& H) {
   H = A*transf*invA;
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

void refineTransform(KalmanFilter* kalman, Frame* lastFrame, Frame* frame) {
   cout << endl << "Before: " << frame->transform << endl;
   Mat A, invA;
   cameraMatrixFromParams(frame->img.cols, frame->img.rows, 30, A, invA);
   cameraTransformFromHomography(frame->transform, A, invA, frame->cameraTransform);
   cout << "Camera transform: " << frame->cameraTransform << endl;
   cout << "Last camera transform: " << lastFrame->cameraTransform << endl;
   Mat transform = lastFrame->cameraTransform * frame->cameraTransform;
   cout << "Wanted transform: " << transform << endl;
   Mat desiredPose = transform * Mat::eye(3,4,CV_64F);
   kalman->predict(); 
   //TODO: I get kalmanPose. Need to get transform
   Mat kalmanPose = kalman->correct(desiredPose.reshape(0,12));
   kalmanPose = kalmanPose.reshape(0,6);
   cout << "KalmanPose: " << kalmanPose << endl;
   kalmanPose.resize(3);
   Mat transformKF = kalmanPose.colRange(0,3);
   cout << "TransformKF: " << transformKF << endl;
   frame->cameraTransform = transformKF;
   homographyFromCameraTransform(frame->cameraTransform, A, invA, frame->transform);
   cout << "After: " << frame->transform << endl;
}

void stabilize(KalmanFilter* kalman, Frame* lastFrame, Frame* frame) {
   findFeatures(lastFrame);
   findTransform(lastFrame, frame);
   //refineTransform(lastFrame, frame);
   refineTransform(kalman, lastFrame, frame);
   // Apply transformation
   warpPerspective(frame->img, frame->stabImg, frame->transform, frame->img.size());
   // Draw circles around detected features.
   for(uint i = 0; i < frame->features.size(); i++) {
      circle(frame->img,frame->features[i],10,-1);
   }
}

class StabilizerData {
public:
   Frame* frame;
   Frame* prevFrame;
   int width;
   int height;
   KalmanFilter* kalman;
};

void setupKalman(KalmanFilter* kf) {
   kf->init(24,12,0,CV_64F);
   Mat trans(24,24,CV_64F);
   for(int i=0;i<24;i++) { trans.at<double>(i,i) = 1; if(i<12) trans.at<double>(i,i+12)=1; }
   Mat processNoise = (Mat_<double>(1,32) << 1e-5, 1e-5, 1e-5, 1e-5
                                           , 1e-5, 1e-5, 1e-5, 1e-5
                                           , 1e-5, 1e-5, 1e-5, 1e-5
                                           , 1e-8, 1e-8, 1e-8, 1e-8
                                           , 1e-8, 1e-8, 1e-8, 1e-8
                                           , 1e-8, 1e-8, 1e-8, 1e-8);
   Mat measurementNoise = (Mat_<double>(1,12) << 1e-3, 1e-3, 1e-3, 1e-3
                                               , 1e-3, 1e-3, 1e-3, 1e-3
                                               , 1e-3, 1e-3, 1e-3, 1e-3);
   kf->transitionMatrix = trans;
   cout << kf->transitionMatrix << endl;
   setIdentity(kf->measurementMatrix);
   setIdentity(kf->processNoiseCov);
   kf->statePost = (Mat_<double>(24,1) << 1,0,0,0
                                         ,0,1,0,0
                                         ,0,0,1,0
                                         ,0,0,0,0
                                         ,0,0,0,0
                                         ,0,0,0,0);
   //for(int i=0;i<32;i++) { kf->processNoiseCov.at<double>(i,i) = processNoise.at<double>(i); }
   kf->processNoiseCov = Mat::eye(24,24,CV_64F)*1e-3;
   cout << kf->processNoiseCov << endl;
   setIdentity(kf->measurementNoiseCov);
   //for(int i=0;i<16;i++) { kf->measurementNoiseCov.at<double>(i,i) = measurementNoise.at<double>(i); }
   kf->measurementNoiseCov = Mat::eye(12,12,CV_64F)*1e-5;
   cout << kf->measurementNoiseCov << endl;
}

Stabilizer::Stabilizer(int width, int height) {
   data = new StabilizerData();
   data->width = width;
   data->height = height;
   data->kalman = new KalmanFilter(32,16,0,CV_64F);
   setupKalman(data->kalman);
}

Stabilizer::~Stabilizer() {
   if(data->frame) delete data->frame;
   if(data->prevFrame) delete data->prevFrame;
   if(data->kalman) delete data->kalman;
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
      stabilize(data->kalman, data->prevFrame, data->frame);
   }
   return 0;
}

void* Stabilizer::getStabilizedImage() { return this->data->frame->stabImg.ptr(); }

void* Stabilizer::getOriginalImage() { return this->data->frame->img.ptr(); }
