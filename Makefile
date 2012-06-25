CC = gcc
CXX = g++
CFLAGS = -Wall
CXXFLAGS = -D__STDC_CONSTANT_MACROS $(CFLAGS)
INCPATH = -I/usr/local/x86_64-unknown-linux-gnu/include
LINK = g++
LFLAGS = -L/usr/local/x86_64-unknown-linux-gnu/lib
LIBS = -lavcodec -lavformat -lavdevice -lswscale -lavutil -lx264

all: stabilize

stabilize:
	$(CXX) -o stabilize $(CXXFLAGS) $(INCPATH) $(LFLAGS) $(LIBS) main.cpp

clean:
	rm stabilize
