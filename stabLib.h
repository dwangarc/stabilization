#include <opencv2/opencv.hpp>

struct Frame {
   cv::Mat img;
   cv::Mat grayImg;
   cv::Mat stabImg;
   std::vector<cv::Point2f> features;
   bool isEjected;
   int numOfStatic;
   cv::Mat transformToPrev;
   cv::Mat transformToOrig;
};

Frame initFrame(cv::Mat img);

void stabilize(Frame& lastFrame, Frame& frame);
