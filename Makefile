CC = gcc
CXX = g++
CFLAGS = -Wall
CXXFLAGS =  $(CFLAGS)
INCPATH = -I/usr/include
LINK = g++
LFLAGS = -L/usr/lib
LIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_calib3d

all: stabilize

stabilize:
	$(CXX) -o stabilize $(CXXFLAGS) $(INCPATH) $(LFLAGS) $(LIBS) main.cpp

clean:
	rm -f stabilize
