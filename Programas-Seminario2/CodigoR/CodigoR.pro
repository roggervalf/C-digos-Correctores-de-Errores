#-------------------------------------------------
#
# Project created by QtCreator 2018-03-31T21:06:59
#
#-------------------------------------------------

QT       += core gui axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CodigoR
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    codigos.cpp

HEADERS += \
        mainwindow.h \
    codigos.h

FORMS += \
        mainwindow.ui

win32:{
INCLUDEPATH += $$(OPENCV_SDK_DIR)\include

LIBS += -L$$(OPENCV_SDK_DIR)\x86\mingw\lib \
        -lopencv_core320        \
        -lopencv_highgui320     \
        -lopencv_imgcodecs320   \
        -lopencv_imgproc320     \
        -lopencv_features2d320  \
        -lopencv_calib3d320     \
        -lopencv_videoio320
}

unix:!macx{
CONFIG+=link_pkgconfig
PKGCONFIG+=opencv
}

unix:macx{
INCLUDEPATH+="/usr/local/include"
LIBS+=-L"/usr/local/lib" \
      -lopencv_world
}

QMAKE_CXXFLAGS += -std=gnu++11
