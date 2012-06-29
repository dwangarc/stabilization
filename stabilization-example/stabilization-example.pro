TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp

LIBS += -L../stabilization/release -lstabilization
INCLUDEPATH += ../stabilization/

LIBS+= -L../opencv/lib -lopencv_calib3d241 -lopencv_core241 -lopencv_highgui241 -lopencv_video241 -lopencv_imgproc241
INCLUDEPATH += ../opencv/include
