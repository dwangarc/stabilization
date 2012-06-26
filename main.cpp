#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

struct Frame {
   Mat img;
   Mat grayImg;
   Mat stabImg;
   vector<Point2f> features;
   bool isEjected;
};

Frame initFrame(Mat img) {
   Frame frame;
   frame.img = img.clone();
   frame.stabImg = img.clone();
   cvtColor(frame.img,frame.grayImg,CV_BGR2GRAY);
   frame.isEjected = false;
   return frame;
}

void findFeatures(Frame& frame) {
   if(frame.isEjected || frame.features.size() < 0.8*MAX_CORNER_COUNT)
      goodFeaturesToTrack( frame.grayImg, frame.features
                         , CORNERS_MAX_COUNT, CORNERS_QUALITY, CORNERS_MIN_DISTANCE
                         , CORNERS_MASK, CORNERS_BLOCK_SIZE, CORNERS_USE_HARRIS
                         , CORNERS_HARRIS_PARAM);
   cornerSubPix(frame.grayImg, frame.features, CORNERS_WIN_SIZE, CORNERS_DEAD_SIZE
               , TermCriteria(TermCriteria::COUNT+TermCriteria::EPS
                             ,CORNERS_ITER_COUNT, CORNERS_ITER_EPS));
   for(int i = 0; i < frame.features.rows; i++) {
      const float* row = frame.features.ptr<float>(i);
      circle(frame.img,Point(row[0],row[1]),10,-1);
   }
}

Mat findTransform(Frame& last_frame, Frame& frame, vector<uchar> status) {
   if(last_frame.features.size() != frame.features.size())
      return MATRIX_IDENTITY;
   for(int i = 0; i < status.size(); i++) {
      if(!status[i]) {
         frame.features.erase(frame.features.begin()+i);
         last_frame.features.erase(last_frame.features.begin()+i);
         status.erase(status.begin()+i);
         i--;
      }
   }
   if(status.size() < 4) { return MATRIX_IDENTITY; }
   return findHomography(last_frame.features, frame.features, CV_RANSAC);
}

void stabilize(Frame& last_frame, Frame& frame) {
   findFeatures(last_frame);
   vector<float> errFeatures;
   vector<uchar> statusFeatures;
   calcOpticalFlowPyrLK( last_frame.grayImg, frame.grayImg
                       , last_frame.features, frame.features
                       , statusFeatures, errFeatures, LK_WIN_SIZE, LK_MAX_LEVEL
                       , TermCriteria( TermCriteria::COUNT+TermCriteria::EPS
                                     , LK_ITER_COUNT, LK_ITER_EPS)
                       , LK_FLAGS, LK_MIN_EIG_THRESHOLD);
   Mat transform = findTransform(last_frame, frame, statusFeatures);
   int res = estimateTransform();
   if(!res) frame.isEjected = true;
   else warpPerspective(frame.img, frame.stabImg, transform, frame.img.size());
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
   if(argc != 2) { cout << "Need file(or device #) for input" << endl; return -1; }
   VideoCapture input(argv[1]);
   if(!input.isOpened()) return -1;
   namedWindow("stab",1);
   Mat img;
   Frame frame, lastFrame;
   input >> img;
   lastFrame = initFrame(img);
   for(;;) {
      input >> img;
      frame = initFrame(img);
      stabilize(lastFrame, frame);
      draw("stab",lastFrame.img,frame.stabImg);
      lastFrame = frame;
      if(waitKey(30) >= 0) break;
   }
   return 0;
}
