#include <opencv2/opencv.hpp>

extern "C" {
      #include <libavcodec/avcodec.h>
      #include <libavformat/avformat.h>
      #include <libavdevice/avdevice.h>
      #include <libswscale/swscale.h>
      #include <libswresample/swresample.h>
      #include <libavutil/opt.h>
}

#include <iostream>
using namespace std;

//TODO Should I use exceptions?

//TODO Rename
struct FFNode {
   AVFormatContext* format;
   AVCodecContext* codec;
   AVStream* stream;
};

void initFF() {
   //TODO: Are all of them needed?
   avcodec_register_all();
   avdevice_register_all();
   av_register_all();
   avformat_network_init();
}

void videoWorker(AVFrame* frame) {

}

int openInputFile(char* file, FFNode** node) {
   int ret;
   AVFormatContext* format = avformat_alloc_context();
   ret = avformat_open_input(&format,file,0,0);
   if(ret < 0) { cout << "Can't open file" << endl; return ret; }
   ret = avformat_find_stream_info(format,0);
   if(ret < 0) { cout << "Can't find stream info" << endl; return ret; }
   AVCodec* decoder;
   int streamIndex = av_find_best_stream(format, AVMEDIA_TYPE_VIDEO,-1,-1,&decoder,0);
   if(streamIndex == AVERROR_STREAM_NOT_FOUND) {
      cout << "Can't find stream" << endl;
      return streamIndex;
   } else if(streamIndex == AVERROR_DECODER_NOT_FOUND) {
      cout << "Found stream but don't have the decoder" << endl;
      return streamIndex;
   } else if(streamIndex < 0) {
      cout << "Unrecognised error during stream search" << endl;
      return streamIndex;
   }
   AVCodecContext* codec = format->streams[streamIndex]->codec;
   if(!codec) {
      cout << "This is weird. We found decoder but no codec?" << endl;
      return -1;
   }
   ret = avcodec_open2(codec,decoder,0);
   if (ret < 0) {
      cout << "Can't associate codec with decoder for some reason" << endl;
      return ret;
   }
   *node = new FFNode;
   (*node)->format = format;
   (*node)->codec = codec;
   (*node)->stream = format->streams[streamIndex];
   return 0;
}

int openOutputFile(char* file, FFNode* inputNode, FFNode** node) {
   int ret;
   AVFormatContext* format;
   avformat_alloc_output_context2(&format,0,"mp4",file);
   //TODO Use baseline for better(actual?) real-time
   AVCodec* encoder = avcodec_find_encoder(CODEC_ID_H264);
   if(!encoder) {
      cout << "Can't find specified encoder" << endl;
      return -1;
   }
   AVStream* stream = avformat_new_stream(format,encoder);
   if(!stream) {
      cout << "Can't create stream" << endl;
      return -1;
   }
   AVCodecContext* codec = stream->codec;
   avcodec_get_context_defaults3(codec,encoder);
   //This is really required for some reason
   codec->codec_id = CODEC_ID_H264;
   codec->pix_fmt = encoder->pix_fmts[0];
   codec->width = inputNode->codec->width;
   codec->height = inputNode->codec->height;
   codec->bit_rate = inputNode->codec->bit_rate;
   codec->time_base = inputNode->codec->time_base;
   codec->gop_size = inputNode->codec->gop_size;
   if(format->oformat->flags & AVFMT_GLOBALHEADER)
      codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
   ret = avcodec_open2(codec,encoder,0);
   if(ret < 0) {
      cout << "Can't associate codec with encoder for some reason" << endl;
      return ret;
   }
   ret = avio_open(&format->pb,file,AVIO_FLAG_WRITE);
   if(ret < 0) {
      cout << "Can't open file for writing" << endl;
      return ret;
   }
   ret = avformat_write_header(format, 0);
   if(ret < 0) {
      cout << "Can't write header" << endl;
      return ret;
   }
   (*node)->format = format;
   (*node)->stream = stream;
   (*node)->codec = codec;
   cout << "Just before finishing" << endl;
   return 0;
}

AVFrame* readFrame(FFNode* inputNode) {
   int ret;
   int got_picture = 0;
   AVPacket* pkt;
   AVFrame* frame;
   //TODO Fail if too much reading errors
   while(!got_picture) {
      ret = av_read_frame(inputNode->format,pkt);
      if(ret < 0) { cout << "Can't read frame" << endl; continue; }
      frame = avcodec_alloc_frame();
      ret = avcodec_decode_video2(inputNode->codec,frame,&got_picture,pkt);
      if(ret < 0) { cout << "Can't decodec packet" << endl; got_picture = 0; continue; }
   }
   frame->pts = av_frame_get_best_effort_timestamp(frame);
   av_free_packet(pkt);
   return frame;
}

void writeFrame(FFNode* outputNode, AVFrame* frame) {
   int ret;
   AVPacket* pkt = new AVPacket;
   av_init_packet(pkt);
   pkt->data = 0;
   pkt->size = 0;
   int got_packet;
   ret = avcodec_encode_video2(outputNode->codec,pkt,frame,&got_packet);
   if(ret < 0) cout << "Can't encode frame" << endl;
   if(got_packet) {
      if(pkt->pts != AV_NOPTS_VALUE) 
         pkt->pts = av_rescale_q( pkt->pts,outputNode->codec->time_base
                                , outputNode->stream->time_base);
      if(pkt->dts != AV_NOPTS_VALUE) 
         pkt->dts = av_rescale_q( pkt->dts,outputNode->codec->time_base
                                , outputNode->stream->time_base);
      if(outputNode->codec->coded_frame->key_frame) pkt->flags|=AV_PKT_FLAG_KEY;
      pkt->stream_index = outputNode->stream->index;
      ret = av_interleaved_write_frame(outputNode->format,pkt);
      if(ret < 0) cout << "Couldn't write packet" << endl;
      av_free_packet(pkt);
   }
}

int transcode(FFNode* inputNode, FFNode* outputNode) {
   int ret;
   SwsContext* scaler = sws_getCachedContext( 0
                                            , inputNode->codec->width
                                            , inputNode->codec->height
                                            , inputNode->codec->pix_fmt
                                            , outputNode->codec->width
                                            , outputNode->codec->height
                                            , outputNode->codec->pix_fmt
                                            , SWS_BICUBIC, 0, 0, 0);
   if(!scaler) { cout << "Can't build scaler" << endl; return -1; }

   AVFrame* frame = readFrame(inputNode);
   AVFrame* newFrame = avcodec_alloc_frame();
   if(!newFrame) {
      cout << "Can't create new AVFrame(out of memory?)." << endl;
      return -1;
   }
   ret = avpicture_alloc( (AVPicture*)newFrame
                  , outputNode->codec->pix_fmt,outputNode->codec->width
                  , outputNode->codec->height);
   if(ret < 0) { cout << "Can't allocate memory for frame" << endl; return -1; }
   //TODO returns newFrame->height?
   sws_scale( scaler,frame->data,frame->linesize,0,frame->height
            , newFrame->data,newFrame->linesize); 
   newFrame->width = frame->width;
   newFrame->height = frame->height;
   newFrame->pts = frame->pts;
   writeFrame(outputNode, newFrame); 
}

int main(int argc, char *argv[]) {
   int ret;
   if(argc != 3) { cout << "Need file for input and output" << endl; return -1; }
   initFF();
   FFNode *inputNode, *outputNode;
   ret = openInputFile(argv[1], &inputNode);
   if(ret < 0) {
      cout << "Input file opening failed" << endl;
      return ret;
   }
   ret = openOutputFile(argv[2], inputNode, &outputNode);
   cout << "Opened output" << endl;
   if(ret < 0) {
      cout << "Output file opening failed" << endl;
      return ret;
   }
   transcode(inputNode,outputNode);

   return 0;
}
