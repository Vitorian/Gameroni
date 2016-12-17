#include "OpenCVScreen.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QMessageBox>
//#include "targetfinder.h"
#include <iostream>


OpenCVScreen::OpenCVScreen(QWidget *parent) :
    QWidget(parent)
{
    cap.open( "D:\\Videos\\2016-11-23 14-17-04.flv" );
    if ( ! cap.isOpened() ) {
        QMessageBox::critical( this, tr("Open video" ), tr("Could not open video file" ));
    }
    //Set the height and width for the captured frame
    //cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
    //cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    cap >> Raw_Image; // Object created using openCV to capture the frames and store the current frame into the Mat object called 'Raw_Image'
    startTimer(1000/30);
    counter =0;
}

void OpenCVScreen::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QBrush brush;
    QPen pen;
    pen.setColor(Qt::red);
    brush.setColor(qRgba(255,0,0,100));
    brush.setStyle(Qt::Dense7Pattern);
    p.setPen(pen);
    p.setBrush(brush);


    QImage image((uchar*)Display_Image.data, Display_Image.cols, Display_Image.rows,QImage::Format_RGB888);
    p.drawImage(rect(),image,image.rect());
}

void OpenCVScreen::timerEvent(QTimerEvent *)
{
    //std::cerr << "Capture" << std::endl;
    if ( cap.isOpened() ) {
        cap >> Raw_Image;
        counter++;
        char text[50];
        sprintf( text, "Frame %d", counter );
        putText( Raw_Image, text, cv::Point2i(0,100), 1, 1, cv::Scalar(0,255,0), 2 );
        Raw_Image.copyTo(Display_Image);

        cvtColor(Display_Image,Display_Image,CV_BGR2RGB);
    }
    repaint();
}

void OpenCVScreen::mousePressEvent(QMouseEvent *)
{
}

void OpenCVScreen::mouseReleaseEvent(QMouseEvent *)
{
}

void OpenCVScreen::mouseMoveEvent(QMouseEvent *)
{
}
