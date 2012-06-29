TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp

unix: DEFINES += LINUX

win32:LIBS += -L../stabilization/release -lstabilization
unix:LIBS += -L../stabilization/ -lstabilization
INCLUDEPATH += ../stabilization/

win32:LIBS+= -L../opencv/lib -lopencv_calib3d241 -lopencv_core241 -lopencv_highgui241 -lopencv_video241 -lopencv_imgproc241
win32:INCLUDEPATH += ../opencv/include

unix:LIBS+=-lopencv_calib3d -lopencv_core -lopencv_highgui -lopencv_video -lopencv_imgproc
