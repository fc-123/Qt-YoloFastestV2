QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    videoyolodetection.cpp \
    yolo-fastestv2.cpp

HEADERS += \
    videoyolodetection.h \
    yolo-fastestv2.h

FORMS += \
    videoyolodetection.ui

INCLUDEPATH +=G:/OpenCV-MinGW-Build-OpenCV-4.1.1-x64/include
LIBS += G:/OpenCV-MinGW-Build-OpenCV-4.1.1-x64/x64/mingw/lib/libopencv_*.a

INCLUDEPATH +=G:/QT_Project/CHYolo-fastestv2/install/include/ncnn
LIBS += G:/QT_Project/CHYolo-fastestv2/install/lib/libncnn.a
LIBS += F:/Qt/Tools/mingw730_64/bin/libgomp-1.dll

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
