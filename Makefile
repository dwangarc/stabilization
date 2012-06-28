CROSS_PREFIX=

CC = $(CROSS_PREFIX)gcc
CXX = $(CROSS_PREFIX)g++
CFLAGS = -Wall
CXXFLAGS = $(CFLAGS)
INCPATH =
LINK = $(CROSS_PREFIX)g++ 
LFLAGS = 
LIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_calib3d

all: stabilize

stabilize: main.o stabilize.o
	$(LINK) $(LFLAGS) -o stabilize main.o stabilize.o $(LIBS)

main.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) main.cpp

stabilize.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) stabilize.cpp

clean:
	rm -f stabilize
	rm -f main.o stabilize.o
