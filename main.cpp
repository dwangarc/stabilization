#include "stabilization.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;
using namespace std;

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
   FILE* file = stdin;
   //FILE* file = fopen("out.video", "rb");
   uint8_t* data;
   int width = 640;
   int height = 480;
   int length = 3*width*height;

   data = new uint8_t[length];
   fread(data, 1, length, file);
   Frame *lastFrame = new Frame(width, height, data);
   delete [] data;

   namedWindow("stab",1);
   for(;;) {
      data = new uint8_t[length];
      fread(data, 1, length, file);
      Frame *frame = new Frame(width, height, data);
      delete [] data;
      stabilize(lastFrame, frame);
      delete lastFrame;
      draw( "stab", width, height
          , frame->getOriginalImage(), frame->getStabilizedImage());
      lastFrame = frame;
      if(waitKey(30) >= 0) break;
   }
   delete lastFrame;
   return 0;
}
