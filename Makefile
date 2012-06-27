#CROSS_PREFIX=i686-w64-mingw32-
#PREFIX=/usr/local/i686-w64-mingw32/
#TARGET=stabilization.exe
CROSS_PREFIX=
PREFIX=/usr
TARGET=stabilize

CC = $(CROSS_PREFIX)gcc
CXX = $(CROSS_PREFIX)g++
CFLAGS = -Wall
CXXFLAGS = $(CFLAGS)
INCPATH = -I$(PREFIX)/include
LINK = $(CROSS_PREFIX)g++ 
LFLAGS = -L$(PREFIX)/lib
#LIBS = -lopencv_core241 -lopencv_highgui241 -lopencv_imgproc241 -lopencv_video241 -lopencv_calib3d241
LIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_calib3d

all: $(TARGET)

$(TARGET): stabLib.o main.o
	$(LINK) $(LFLAGS) -o $(TARGET) main.o stabLib.o $(LIBS)

stabLib.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) stabLib.cpp

main.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) main.cpp

clean:
	rm -f $(TARGET)
	rm -f main.o stabLib.o
