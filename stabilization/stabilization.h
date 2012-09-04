#pragma once

#include <stdint.h>

#if defined(LINUX)
#define DECL_EXPORT
#elif defined(LIBSTABILIZATION)
#define DECL_EXPORT __declspec(dllexport)
#else
#define DECL_EXPORT __declspec(dllimport)
#endif

class StabilizerData;

class DECL_EXPORT Stabilizer {
public:
   // When using defaults width and height would have to be given with the first frame
   Stabilizer(int width = -1, int height = -1);
   ~Stabilizer();
   // Width and height must be the same for all frames.
   // If this is the first frame and Stabilizer was constructed without specifying
   // width and height, you have to specify them here.
   // Image must be BGR3 and it makes a copy of it.
   int addFrame(void *image, int width = -1, int height = -1);
   // Will give a pointer to internal image in BGR3
   void* getStabilizedImage();
   // Will give a pointer to internal image in BGR3
   void* getOriginalImage();
   // Turn stabilization on/off
   void setStabilization(bool state);
private:
   StabilizerData* data;
};
