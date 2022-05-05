#define _CRT_SECURE_NO_WARNINGS
#include "yolo-fastestv2.h"
#include "videoyolodetection.h"
#include "ui_videoyolodetection.h"

#include <opencv2\core\core.hpp>
#include "opencv2\imgproc\types_c.h"

#include <QWidget>
#include <QImage>
#include <QTimer>

#include<stdlib.h>
#include<random>
#include <QDebug>
#include <QDir>
#include <QFileDialog>


#define QS(qstr) std::string(qstr2str(QString(qstr)))

inline std::string qstr2str(const QString& qstr)
{
    QByteArray cdata = qstr.toLocal8Bit();
    return std::string(cdata);
}

VideoYoloDetection::VideoYoloDetection(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VideoYoloDetection)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(openCamara()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(takingPictures()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(closeCamara()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(yoloDetection()));
    setWindowTitle(tr("YOLO 目标检测"));

    timer = new QTimer(this);
    imag = new QImage();
    connect(timer, SIGNAL(timeout()), this, SLOT(getFrame()));//超时就读取当前摄像头信息
}

VideoYoloDetection::~VideoYoloDetection()
{
    delete ui;
}

void VideoYoloDetection::openCamara()
{

    //------------打开文件的方式--------------//
    QString vedioFilePath;
    vedioFilePath = QFileDialog::getOpenFileName(
        this,
        tr("选择视频文件"),
        QDir::currentPath(),
        "All files(*.mp4 *.MP4 *.AVI *.avi)");

    if (QS(vedioFilePath).empty())
    {
        qDebug() << "The VedioFilePath is not exit!!!\n";
        return;
    }

    if (this->capture1.isOpened())
    {
        this->capture1.release();
    }
    if (!this->capture1.open(QS(vedioFilePath)))
    {
            qDebug() << "open video false 0";
    }

    timer->start(10);
}

void VideoYoloDetection::getFrame()
{
    capture1 >> showimage;

    using namespace::std;
    double fps;
    char str[10];
    double t = 0;
    //类别名称、需要替换自己的

    static const char* class_names[] = {
        "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
        "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
        "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
        "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
        "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
        "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
        "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
        "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
        "hair drier", "toothbrush"
    };



    yoloFastestv2 api;
//    api.loadModel("G:/ncnn-master/build-vs2019/tools/onnx/yolo-Fastestv2/yolo-fastestV2_2-sim.param",
//                  "G:/ncnn-master/build-vs2019/tools/onnx/yolo-Fastestv2/yolo-fastestV2_2-sim.bin");

    api.loadModel("G:\\pycharm_pytorch171\\detection\\Yolo-FastestV2-main\\sample\\ncnn\\model\\yolo-fastestv2-opt.param",
                  "G:\\pycharm_pytorch171\\detection\\Yolo-FastestV2-main\\sample\\ncnn\\model\\yolo-fastestv2-opt.bin");


    std::vector<TargetBox> boxes;
//        api.detection(cvImg, boxes);

    t = (double)cv::getTickCount();
    api.detection(this->showimage, boxes);

    for (std::size_t i = 0; i < boxes.size(); i++)
    {
        std::cout << boxes[i].x1 << " " << boxes[i].y1 << " " << boxes[i].x2 << " " << boxes[i].y2
            << " " << boxes[i].score << " " << boxes[i].cate << std::endl;

        char text[256];
        sprintf(text, "%s %.1f%%", class_names[boxes[i].cate], boxes[i].score * 100);

        int baseLine = 0;
        cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

        int x = boxes[i].x1;
        int y = boxes[i].y1 - label_size.height - baseLine;
        if (y < 0)
            y = 0;
        if (x + label_size.width > this->showimage.cols)
            x = this->showimage.cols - label_size.width;

        cv::rectangle(this->showimage, cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)),
            cv::Scalar(255, 255, 255), -1);

        cv::putText(this->showimage, text, cv::Point(x, y + label_size.height),
            cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));

        cv::rectangle(this->showimage, cv::Point(boxes[i].x1, boxes[i].y1),
            cv::Point(boxes[i].x2, boxes[i].y2), cv::Scalar(255, 255, 0), 2, 2, 0);



    }

    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    fps = 1.0 / t;
    sprintf(str, "%.2f", fps);      // 帧率保留两位小数
    std::string fpsString("FPS:");
    fpsString += str;        // 在"FPS:"后加入帧率数值字符串



    //将帧率信息写在输出帧上
    cv::putText(this->showimage,                  // 图像矩阵
        fpsString,                  // string型文字内容
        cv::Point(20, 20),           // 文字坐标，以左下角为原点
        cv::FONT_HERSHEY_SIMPLEX,   // 字体类型
        0.5,                    // 字体大小
        cv::Scalar(0, 0, 255)); //BGR

    QImage imag2 = Mat2Qimage(this->showimage);

    ui->label->setScaledContents(true);
    ui->label->setPixmap(QPixmap::fromImage(imag2));

}


void VideoYoloDetection::closeCamara()
{
    timer->stop();
    ui->label->clear();
    capture1.release();
}

std::string strRand(int length) {			// length: 产生字符串的长度
    char tmp;							// tmp: 暂存一个随机数
    std::string buffer;						// buffer: 保存返回值


    std::random_device rd;					// 产生一个 std::random_device 对象 rd
    std::default_random_engine random(rd());	// 用 rd 初始化一个随机数发生器 random

    for (int i = 0; i < length; i++) {
        tmp = random() % 36;
        if (tmp < 10) {
            tmp += '0';
        }
        else {
            tmp -= 10;
            tmp += 'A';
        }
        buffer += tmp;
    }
    return buffer;
}

//拍照
void VideoYoloDetection::takingPictures()
{
//    capture1.open(0); //1改成0
//    capture1.open("I:/work/W6013/avi/test/test.avi");
    capture1 >> showimage;
    QImage img = Mat2Qimage(showimage);
    ui->label_2->setScaledContents(true);
    ui->label_2->setPixmap(QPixmap::fromImage(img));

    std::string writePath = "G:\\QT_Project\\CHVideoYoloDetection\\image\\";
    std::string name;
//    int i = 0;
    name = writePath + strRand(4) + std::to_string(i) + ".jpg";
    i++;
    imwrite(name, showimage);


}

QImage VideoYoloDetection::Mat2Qimage(cv::Mat cvImg)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if (cvImg.type() == CV_8UC1)
    {
        QImage image(cvImg.cols, cvImg.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for (int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = cvImg.data;
        for (int row = 0; row < cvImg.rows; row++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, cvImg.cols);
            pSrc += cvImg.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if (cvImg.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)cvImg.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, cvImg.cols, cvImg.rows, cvImg.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if (cvImg.type() == CV_8UC4)
    {
//		qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)cvImg.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, cvImg.cols, cvImg.rows, cvImg.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
//		qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}



void VideoYoloDetection::yoloDetection()
{
    using namespace::std;
    double fps;
    char string[10];
    double t = 0;
    static const char* class_names[] = {
        "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
        "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
        "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
        "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
        "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
        "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
        "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
        "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
        "hair drier", "toothbrush"
    };

    yoloFastestv2 api;

//    api.loadModel("G:/ncnn-master/build-vs2019/tools/onnx/yolo-Fastestv2/yolo-fastestV2_2-sim.param",
//                  "G:/ncnn-master/build-vs2019/tools/onnx/yolo-Fastestv2/yolo-fastestV2_2-sim.bin");

    api.loadModel("G:\\pycharm_pytorch171\\detection\\Yolo-FastestV2-main\\sample\\ncnn\\model\\yolo-fastestv2-opt.param",
                  "G:\\pycharm_pytorch171\\detection\\Yolo-FastestV2-main\\sample\\ncnn\\model\\yolo-fastestv2-opt.bin");


    QString imageFilePath;
    imageFilePath = QFileDialog::getOpenFileName(
        this,
        tr("选择图片文件"),
        QDir::currentPath(),
        "All files(*.jpg *JPG *.jpeg *.png *.PNG *.gif *.bmp)");																						// 数据检查
    if (QS(imageFilePath).empty())
    {
        qDebug()<< "The imageFilePath is not exit!!!\n";
        return;
    }

    cv::Mat inputImage = cv::imread(QS(imageFilePath));

    std::vector<TargetBox> boxes;

    api.detection(inputImage, boxes);

    for (std::size_t i = 0; i < boxes.size(); i++)
    {
        std::cout << boxes[i].x1 << " " << boxes[i].y1 << " " << boxes[i].x2 << " " << boxes[i].y2
            << " " << boxes[i].score << " " << boxes[i].cate << std::endl;

        char text[256];
        sprintf(text, "%s %.1f%%", class_names[boxes[i].cate], boxes[i].score * 100);

        int baseLine = 0;
        cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

        int x = boxes[i].x1;
        int y = boxes[i].y1 - label_size.height - baseLine;
        if (y < 0)
            y = 0;
        if (x + label_size.width > inputImage.cols)
            x = inputImage.cols - label_size.width;

        cv::rectangle(inputImage, cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)),
            cv::Scalar(255, 255, 255), -1);

        cv::putText(inputImage, text, cv::Point(x, y + label_size.height),
            cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));

        cv::rectangle(inputImage, cv::Point(boxes[i].x1, boxes[i].y1),
            cv::Point(boxes[i].x2, boxes[i].y2), cv::Scalar(255, 255, 0), 2, 2, 0);

        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        fps = 1.0 / t;
        sprintf(string, "%.2f", fps);      // 帧率保留两位小数
        std::string fpsString("FPS:");
        fpsString += string;        // 在"FPS:"后加入帧率数值字符串

    }


    QImage imag1 = Mat2Qimage(inputImage);
    imag1.scaled(ui->qLabel->width(), ui->qLabel->height());

//    ui->qLabel->setScaledContents(true);
    ui->qLabel->setPixmap(QPixmap::fromImage(imag1));

    std::string writePath = "G:\\QT_Project\\CHVideoYoloDetection\\image_yolo_detection\\";
    std::string name;

    name = writePath + strRand(4) + std::to_string(i) + ".jpg";
    i++;
    imwrite(name, showimage);

}
