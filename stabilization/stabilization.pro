#-------------------------------------------------
#
# Project created by QtCreator 2012-06-28T12:46:04
#
#-------------------------------------------------

QT       -= core gui

TARGET = stabilization
TEMPLATE = lib
CONFIG += dll
CONFIG -= qt

win32: DEFINES += LIBSTABILIZATION
unix:  DEFINES += LINUX

SOURCES += stabilization.cpp

HEADERS += stabilization.h

win32:LIBS+= -L../opencv/lib -lopencv_calib3d241 -lopencv_core241 -lopencv_highgui241 -lopencv_video241 -lopencv_imgproc241
win32:INCLUDEPATH += ../opencv/include

unix:LIBS+=-lopencv_calib3d -lopencv_core -lopencv_highgui -lopencv_video -lopencv_imgproc

unix {
   isEmpty(PREFIX) {
      PREFIX=/usr/local/
   }
   target.path = $$PREFIX/lib
   headers.files = stabilization.h
   headers.path = $$PREFIX/include
   INSTALLS += headers target
}
