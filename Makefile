CROSS_PREFIX=i686-w64-mingw32-
CC = $(CROSS_PREFIX)gcc
CXX = $(CROSS_PREFIX)g++
CFLAGS = -Wall
CXXFLAGS = $(CFLAGS)
INCPATH = -I/usr/local/i686-w64-mingw32/include
LINK = $(CROSS_PREFIX)g++ -static
LFLAGS = -L/usr/local/i686-w64-mingw32/lib -L/usr/i686-w64-mingw32/lib -L/usr/lib
LIBS = 
EXTRALIBS = -lopencv_core241 -lopencv_highgui241 -lopencv_imgproc241 -lopencv_video241 -lopencv_calib3d241

all: stabilize

stabilize: main.o
	$(LINK) -o stabilize $(LFLAGS) $(LIBS) $(EXTRALIBS) main.o

main.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) main.cpp

clean:
	rm -f stabilize
	rm -f main.o
