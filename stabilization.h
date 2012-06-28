#pragma once

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
   // image should be RGB3
   // TODO: Check what happens when I `delete image`.
   Frame(int width, int height, void* image);
   ~Frame();
   void* getStabilizedImage();
   void* getOriginalImage();
private:
   FrameData* data;
};

void DECL_EXPORT stabilize(Frame lastFrame, Frame frame);
