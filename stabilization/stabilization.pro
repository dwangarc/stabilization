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

DEFINES += LIBSTABILIZATION

SOURCES += stabilization.cpp

HEADERS += stabilization.h

LIBS+= -L../opencv/lib -lopencv_calib3d241 -lopencv_core241 -lopencv_highgui241 -lopencv_video241 -lopencv_imgproc241
INCLUDEPATH += ../opencv/include
