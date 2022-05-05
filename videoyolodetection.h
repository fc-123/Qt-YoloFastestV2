#ifndef VIDEOYOLODETECTION_H
#define VIDEOYOLODETECTION_H

#include <QMainWindow>
#include <opencv2\core\core.hpp>
#include <QWidget>
#include <QImage>
#include <QTimer>     // 设置采集数据的间隔时间
#include <QLabel>

#include <QGraphicsScene>
#include <QGraphicsView>

#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>  //opencv申明

QT_BEGIN_NAMESPACE
namespace Ui { class VideoYoloDetection; }
QT_END_NAMESPACE

class VideoYoloDetection : public QMainWindow
{
    Q_OBJECT

public:
    VideoYoloDetection(QWidget *parent = nullptr);
    ~VideoYoloDetection();

private slots:
    void openCamara();      // 打开摄像头并检测
    void getFrame();       // 读取当前帧信息
    void closeCamara();     // 关闭摄像头。
    void takingPictures();  // 拍照
    void yoloDetection();     //检测一帧的图像

private:
    Ui::VideoYoloDetection *ui;
    QTimer    *timer;
    QImage    *imag;
    CvCapture *cam;// 视频获取结构， 用来作为视频获取函数的一个参数
    cv::Mat  *frame;
    cv::VideoCapture capture1;
    cv::VideoCapture capture2;
    cv::Mat showimage;
    QImage Mat2Qimage(cv::Mat cvImg);

    QLabel *imageLabel;					// 显示图片Label控件
    void setCVMat2QtLabel(const cv::Mat &image, QLabel *qLabel);

    int i = 0; //图片计数
};
#endif // VIDEOYOLODETECTION_H
