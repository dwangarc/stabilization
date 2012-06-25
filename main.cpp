#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

Mat convert(Mat lastFrame, Mat frame, int alg = 0) {//,InputArray lastFeatures,vector<Point> features) {
   Mat lastFeatures;
   Mat features;
   Mat stabFrame = frame.clone();
   Mat frameGray;
   Mat lastFrameGray;
   cvtColor(frame,frameGray,CV_BGR2GRAY);
   cvtColor(lastFrame,lastFrameGray,CV_BGR2GRAY);
   if(alg == 0) {
      // Corner detection
      int maxCount = 100;
      double qLevel = 0.01;
      double minDistance = 50;
      InputArray mask = noArray();
      int blockSize = 3;
      bool useHarrisDetector = false;
      double k = 0.04;


      goodFeaturesToTrack( lastFrameGray,lastFeatures,maxCount,qLevel,minDistance
                         , mask,blockSize,useHarrisDetector,k);
      cornerSubPix(lastFrameGray,lastFeatures,Size(5,5),Size(-1,-1),TermCriteria(TermCriteria::COUNT+TermCriteria::EPS,50,0.001));
      // Lucas-Kanade
      Mat status, err;
      Size winSize(30,30);
      int maxLevel = 4;
      TermCriteria criteria(TermCriteria::COUNT+TermCriteria::EPS,50,0.001);
      int flags = 0;//OPTFLOW_USE_INITIAL_FLOW;
      double minEigThreshold = 1e-4;

      calcOpticalFlowPyrLK( lastFrameGray, frameGray, lastFeatures, features, status, err
                          , winSize, maxLevel, criteria, flags, minEigThreshold);
      for(int i = 0; i < lastFeatures.rows; i++) {
         const float* row = lastFeatures.ptr<float>(i);
         //cout << row[0] << " " << row[1] << endl;
         circle(lastFrame,Point(row[0],row[1]),10,-1);
      }
   } else if(alg == 1) {
      // Whole image is used
      double pyr_scale = 0.5;
      int levels = 2;
      int winsize = 30;
      int iters = 20;
      int poly_n = 5;
      double poly_sigma = 1.1;
      int flags = 0;
      Mat flow;
      calcOpticalFlowFarneback(lastFrameGray, frameGray, flow, pyr_scale, levels, winsize, iters, poly_n, poly_sigma, flags);
      vector<Point> features1, features2;
      for(int i = 0; i < lastFrameGray.rows; i++) {
         for(int j = 0; j < lastFrameGray.cols; j++) {
            features1.push_back(Point(i,j));
            int k = flow.at<int>(i,j,0);
            int l = flow.at<int>(i,j,1);
            features2.push_back(Point(k,l));
         }
      }
      lastFeatures = ((InputArray)features1).getMat();
      features = ((InputArray)features2).getMat();
   }

   Mat trans = findHomography(lastFeatures, features);
   warpPerspective(frame,stabFrame,trans,stabFrame.size());

   return stabFrame;
}

//Images must have same type
void draw(char* windowName, Mat img1, Mat img2) {
   Mat res(max(img1.rows,img2.rows),img1.cols+img2.cols,img1.type());
   img1.copyTo(res(Rect(Point(0,0),img1.size())));
   img2.copyTo(res(Rect(Point(img1.cols,0),img2.size())));
   Mat resSmall;
   resize(res,resSmall,Size(1000,300));
   imshow(windowName,resSmall);
}

int main(int argc, char *argv[]) {
   int ret;
   if(argc != 2) { cout << "Need file for input" << endl; return -1; }

   VideoCapture input(argv[1]);
   //VideoCapture input(0);
   if(!input.isOpened())
      return -1;
   namedWindow("stab",1);
   Mat frame, lastFrame, stabFrame;
   input >> frame;
   lastFrame = frame.clone();
   for(;;) {
      input >> frame;
      stabFrame = convert(lastFrame, frame, 0);//, lastFeatures, features);
      draw("stab",lastFrame,stabFrame);
      lastFrame = frame.clone();
      //lastFrame = stabFrame.clone();
      if(waitKey(30) >= 0) break;
   }
   return 0;
}
