#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

Mat convert(Mat frame) {
   return frame;
}

int main(int argc, char *argv[]) {
   int ret;
   if(argc != 2) { cout << "Need file for input" << endl; return -1; }

   VideoCapture input(argv[1]);
   if(!input.isOpened())
      return -1;
   namedWindow("edges",1);
   for(;;) {
      Mat frame;
      input >> frame;
      Mat orig,conv;
      resize(frame,orig,Size(320,240));
      conv = convert(orig);
      Mat res(orig.rows,orig.cols*2,orig.type());
      orig.copyTo(res(Rect(Point(0,0),orig.size())));
      conv.copyTo(res(Rect(Point(conv.cols,0),conv.size())));
      imshow("edges",res);
      if(waitKey(30) >= 0) break;
   }
   return 0;
}
