/*-----------------------------------------------------------------------------
 * This file is part of Gameroni -
 * http://www.vitorian.com/x1/archives/419
 * https://github.com/hftrader/gameroni
 *
 * Gameroni is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gameroni is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * -----------------------------------------------------------------------------*/

#include "windowcapturewidget.h"
#include "WindowCaptureWidget.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileInfo>
#include <QMap>
#include <iostream>

#include "windowcapture.h"
#include "imageprocessorlaplacian.h"
#include "imageprocessormotion.h"
#include "win32utils.h"


WindowCaptureWidget::WindowCaptureWidget(QWidget *parent) : QWidget(parent)
{
    counter = 0;
    avg_time = 0;

    _processor_list.push_back( std::shared_ptr<ImageProcessor>(new ImageProcessorLaplacian) );
    _processor_list.push_back( std::shared_ptr<ImageProcessor>(new ImageProcessorMotion) );
    _processor = _processor_list[0];
    startTimer(1);
}

void WindowCaptureWidget::updateWindow( HWND hWnd )
{
    _cap.endCapture();
    _cap.startCapture( hWnd, true );
}

void WindowCaptureWidget::updateState( const QVariantMap& vmap )
{
    if ( _processor )
        _processor->updateState( vmap );
}


void WindowCaptureWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QBrush brush;
    QPen pen;
    pen.setColor(Qt::red);
    brush.setColor(qRgba(255,0,0,100));
    brush.setStyle(Qt::Dense7Pattern);
    p.setPen(pen);
    p.setBrush(brush);

    if ( !final_img.empty() ) {
        QImage image((uchar*)final_img.data, final_img.cols, final_img.rows,
                     final_img.elemSize()==3 ? QImage::Format_RGB888 : QImage::Format_RGB32 );
        p.drawImage(rect(),image,image.rect());
    }
}


void WindowCaptureWidget::timerEvent(QTimerEvent *)
{
    cv::Mat raw_img;
    if ( _cap.captureFrame( raw_img ) )
    {
        // Process image
        uint64_t t0 = __rdtsc();
        if ( _processor )
            _processor->process( raw_img, final_img, counter );
        uint64_t elap = __rdtsc() - t0;

        // Compute timing stats
        const int num_loops = 100;
        avg_time += elap;
        if ( ++counter % num_loops == 0 ) {
            std::cout  << " Elapsed:" << avg_time/num_loops << std::endl;
            avg_time = 0;
        }
        repaint();
    }
}

void WindowCaptureWidget::setAlgorithm( int algo )
{
    _processor = _processor_list[algo];
    counter = 0;
    avg_time = 0;
}


void WindowCaptureWidget::mousePressEvent(QMouseEvent * )
{
  //qDebug() << e->x() << e->y();
}

void WindowCaptureWidget::mouseReleaseEvent(QMouseEvent *)
{
    //qDebug() << x1 << y1;
}

void WindowCaptureWidget::mouseMoveEvent(QMouseEvent *)
{
}
