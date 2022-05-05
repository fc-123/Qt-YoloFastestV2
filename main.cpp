#include "videoyolodetection.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VideoYoloDetection w;
    w.show();
    return a.exec();
}
