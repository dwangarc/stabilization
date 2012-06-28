#pragma once

#include <stdint.h>

#if defined(LINUX)
#define DECL_EXPORT
#elif defined(LIBSTABILIZATION)
#define DECL_EXPORT __declspec(dllexport)
#else
#define DECL_EXPORT __declspec(dllimport)
#endif

class FrameData;

class DECL_EXPORT Frame {
public:
   // image should be BGR3
   // And it makes a copy of image so you should dispose of it yourself
   Frame(int width, int height, void *image);
   ~Frame();
   // Will NOT make a copy of image
   void* getStabilizedImage();
   // Will NOT make a copy of image
   void* getOriginalImage();
   DECL_EXPORT friend void stabilize(Frame *lastFrame, Frame *frame);
private:
   FrameData* data;
};

