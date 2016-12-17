#ifndef OPENCVSCREEN_H
#define OPENCVSCREEN_H

#include <QWidget>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

class OpenCVScreen : public QWidget
{
    Q_OBJECT

public:
    explicit OpenCVScreen(QWidget *parent = 0);

signals:

public slots:
    void paintEvent(QPaintEvent *e);
    void timerEvent(QTimerEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
private:
    cv::Mat Raw_Image, Display_Image; //Display Image is the image that is going to be displayed
    cv::VideoCapture cap;
    int  counter;
};

#endif // OPENCVSCREEN_H
