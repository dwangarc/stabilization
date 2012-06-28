//#include <stabilization.h>
#include <opencv2/opencv.hpp>
extern "C" {
   #include <libavcodec/avcodec.h>
   #include <libavdevice/avdevice.h>
   #include <libavformat/avformat.h>
   #include <libswscale/swscale.h>
}

using namespace cv;
using namespace std;


void draw(char* windowName, int width, int height, void* img1, void* img2) {
   //Mat mimg1(height, width, CV_8UC3, img1);
   //Mat res(max(img1.rows,img2.rows),img1.cols+img2.cols,img1.type());
   //img1.copyTo(res(Rect(Point(0,0),img1.size())));
   //img2.copyTo(res(Rect(Point(img1.cols,0),img2.size())));
   //Mat resSmall;
   //resize(res,resSmall,Size(1000,300));
   //imshow(windowName,resSmall);
}

int main(int argc, char *argv[]) {
   char* fmt = "v4l2";
   char* name = "/dev/video0";
   PixelFormat pix_fmt = PIX_FMT_BGR24;
   //Initializing ffmpeg
   avcodec_register_all();
   avdevice_register_all();
   av_register_all();
   avformat_network_init();
   AVFormatContext* format = avformat_alloc_context();
   if(avformat_open_input(&format,name,av_find_input_format(fmt),0) < 0) {
      cout << "Can't open input" << endl;
      return -1;
   }
   if(avformat_find_stream_info(format,0) < 0) {
      cout << "Can't find stream info" << endl;
      return -1;
   }
   AVCodecContext* codec = format->streams[0]->codec;
   if(!codec) { cout << "Can't get codec context" << endl; return -1; }
   AVCodec* decoder = avcodec_find_decoder(codec->codec_id);
   if(!decoder) { cout << "Can't find decoder" << endl; return -1; }
   if(avcodec_open2(codec,decoder,0) < 0) {
      cout << "Can't associate codec on input" << endl;
      return -1;
   }
   int width = codec->width;
   int height = codec->height;
   //Getting ready to convert
   SwsContext* scaler = sws_getCachedContext( 0, width, height, codec->pix_fmt
                                            , width, height, pix_fmt
                                            , SWS_BICUBIC, 0, 0, 0);
   //Getting first picture
   AVPacket pkt;
   AVFrame *data, *newFrame;
   int got_picture = 0;
   /*
   while(!got_picture) {
      if(av_read_frame(format,&pkt) < 0) cout << "Can't read frame" << endl;
      data = avcodec_alloc_frame();
      if(avcodec_decode_video2(codec,data,&got_picture,&pkt) < 0) {
         cout << "Can't decode pkt" << endl;
         continue;
      }
      if(!got_picture) continue;
      //newFrame = avcodec_alloc_frame();
      //avpicture_alloc((AVPicture*)newFrame,pix_fmt,width,height);
      //sws_scale(scaler,data->data,data->linesize,0,data->height
      //         ,newFrame->data,newFrame->linesize);
      //av_free(data);
      //data = newFrame;
      av_free_packet(&pkt);
   }
   */
   //Frame *frame, *lastFrame;
   //lastFrame = new Frame(width, height, data.data[0]);
   //Inits new window
   namedWindow("stab",1);
   //Main loop
   cout << "Starting main loop" << endl;
   for(;;) {
      got_picture = 0;
      while(!got_picture) {
         if(av_read_frame(format,&pkt) < 0) cout << "Can't read frame" << endl;
         data = avcodec_alloc_frame();
         if(avcodec_decode_video2(codec,data,&got_picture,&pkt) < 0) {
            cout << "Can't decode pkt" << endl;
            continue;
         }
         if(!got_picture) continue;
         newFrame = avcodec_alloc_frame();
         avpicture_alloc((AVPicture*)newFrame,pix_fmt,width,height);
         sws_scale(scaler,data->data,data->linesize,0,data->height
                  ,newFrame->data,newFrame->linesize);
         av_free(data);
         data = newFrame;
         av_free_packet(&pkt);
      }
      draw("stab", width, height, data->data[0], data->data[0]);
      avpicture_free((AVPicture*)data);
      av_free(data);
      //frame = new Frame(width, height, data.data[0]);
      //stabilize(lastFrame, frame);
      //draw("stab", width, height, frame.getOriginalImage(), frame.getStabilizedImage());
      //delete lastFrame;
      //lastFrame = frame;
      if(waitKey(30) >= 0) break;
   }
   //delete lastFrame;
   return 0;
}

/*
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
*/
