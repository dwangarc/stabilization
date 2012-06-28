CC = gcc
CXX = g++
CFLAGS = -Wall -DLINUX
CXXFLAGS = $(CFLAGS) -D__STDC_CONSTANT_MACROS
INCPATH =
LINK = g++ 
LFLAGS = 
LIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_calib3d\
		 -lavcodec -lavformat -lswscale -lavdevice -lavutil

all: stabilize

#stabilize: main.o stabilization.o
#	$(LINK) $(LFLAGS) -o stabilize main.o stabilization.o $(LIBS)

stabilize: main.o
	$(LINK) $(LFLAGS) -o stabilize main.o $(LIBS)

main.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) main.cpp

stabilization.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) stabilization.cpp

clean:
	rm -f stabilize
	rm -f main.o stabilization.o
