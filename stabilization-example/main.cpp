#include "stabilization.h"
//Used here only to display graphics.
#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;
using namespace std;

//The only function using opencv directly
void draw(char* windowName, int width, int height, void* img1, void* img2) {
   Mat mimg1(height, width, CV_8UC3, img1);
   Mat mimg2(height, width, CV_8UC3, img2);
   Mat res(max(mimg1.rows,mimg2.rows),mimg1.cols+mimg2.cols,CV_8UC3);
   mimg1.copyTo(res(Rect(Point(0,0),mimg1.size())));
   mimg2.copyTo(res(Rect(Point(mimg1.cols,0),mimg2.size())));
   Mat resSmall;
   resize(res,resSmall,Size(1000,300));
   imshow(windowName,resSmall);
}

int main(int argc, char *argv[]) {
   //Must be rawvideo 640x480 with BGR3 pixels
   //FILE* file = fopen("out.video", "rb");
   VideoCapture cap;
   if(argc == 1)
      cap = VideoCapture(1);
   else
      cap = VideoCapture(argv[1]);
   Mat frame;
   //uint8_t* data;
   //int width = 640;
   //int height = 480;
   int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
   int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
   int length = 3*width*height;

   Stabilizer *stab = new Stabilizer(width,height);

   //data = new uint8_t[length];
   //fread(data, 1, length, file);
   //stab->addFrame(data);
   cap >> frame;
   stab->addFrame(frame.ptr());
   //delete [] data;

   namedWindow("stab",1);
   for(;;) {
      //data = new uint8_t[length];
      //fread(data, 1, length, file);
      //stab->addFrame(data);
      cap >> frame;
      stab->addFrame(frame.ptr());
      //delete [] data;
      draw( "stab", width, height, stab->getOriginalImage(), stab->getStabilizedImage());
      if(waitKey(30) >= 0) break;
   }
   delete stab;
   return 0;
}
