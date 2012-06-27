#include "stabLib.h"

using namespace cv;
using namespace std;

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
