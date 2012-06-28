#include "stabilization.h"
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
   Mat mimg1(height, width, CV_8UC3, img1);
   Mat mimg2(height, width, CV_8UC3, img2);
   Mat res(max(mimg1.rows,mimg2.rows),mimg1.cols+mimg2.cols,CV_8UC3);
   mimg1.copyTo(res(Rect(Point(0,0),mimg1.size())));
   mimg2.copyTo(res(Rect(Point(mimg1.cols,0),mimg2.size())));
   Mat resSmall;
   resize(res,resSmall,Size(1000,300));
   imshow(windowName,resSmall);
}

int initFFmpeg( char* fmt, char* name
               , AVCodecContext **codec, AVFormatContext **format, SwsContext **scaler) {
   avcodec_register_all();
   avdevice_register_all();
   av_register_all();
   avformat_network_init();
   *format = avformat_alloc_context();
   if(avformat_open_input(format,name,av_find_input_format(fmt),0) < 0) {
      cout << "Can't open input" << endl;
      return -1;
   }
   if(avformat_find_stream_info(*format,0) < 0) {
      cout << "Can't find stream info" << endl;
      return -1;
   }
   *codec = (*format)->streams[0]->codec;
   if(!codec) { cout << "Can't get codec context" << endl; return -1; }
   AVCodec* decoder = avcodec_find_decoder((*codec)->codec_id);
   if(!decoder) { cout << "Can't find decoder" << endl; return -1; }
   if(avcodec_open2(*codec,decoder,0) < 0) {
      cout << "Can't associate codec on input" << endl;
      return -1;
   }
   cout << (*codec)->width << endl << (*codec)->height << endl;
   *scaler = sws_getCachedContext( 0, (*codec)->width, (*codec)->height, (*codec)->pix_fmt
                                , (*codec)->width, (*codec)->height, PIX_FMT_BGR24
                                , SWS_BICUBIC, 0, 0, 0);
   return 0;
}

Frame* readFrame(AVCodecContext* codec, AVFormatContext* format, SwsContext* scaler) {
   AVPacket pkt;
   AVFrame* frame;
   int got_picture = 0;
   while(!got_picture) {
      if(av_read_frame(format,&pkt) < 0) cout << "Can't read frame" << endl;
      frame = avcodec_alloc_frame();
      if(avcodec_decode_video2(codec,frame,&got_picture,&pkt) < 0) {
         cout << "Can't decode pkt" << endl;
         continue;
      }
      if(!got_picture) continue;
      AVFrame* newFrame = avcodec_alloc_frame();
      avpicture_alloc((AVPicture*)newFrame,PIX_FMT_BGR24,codec->width,codec->height);
      sws_scale(scaler,frame->data,frame->linesize,0,frame->height
               ,newFrame->data,newFrame->linesize);
      av_free(frame);
      frame = newFrame;
      av_free_packet(&pkt);
   }
   cout << frame->data[0];
   Frame* packed_frame = new Frame(codec->width,codec->height,frame->data[0]);
   avpicture_free((AVPicture*) frame);
   av_free(frame);
   return packed_frame;
}

int main(int argc, char *argv[]) {
   AVCodecContext *codec;
   AVFormatContext *format;
   SwsContext* scaler;
   if(initFFmpeg("avi","videos/vid1.avi",&codec,&format,&scaler) < 0) return -1;
   Frame *lastFrame = readFrame(codec,format,scaler);
   namedWindow("stab",1);
   for(;;) {
      //cout << "Reading new frame" << endl;
      Frame *frame = readFrame(codec,format,scaler);
      //cout << "Read new frame" << endl;
      stabilize(lastFrame, frame);
      //cout << "Stabilized frame" << endl;
      delete lastFrame;
      draw( "stab", codec->width, codec->height
          , frame->getOriginalImage(), frame->getStabilizedImage());
      lastFrame = frame;
      if(waitKey(30) >= 0) break;
   }
   delete lastFrame;
   return 0;
}
