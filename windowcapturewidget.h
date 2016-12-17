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
#pragma once

#include <QWidget>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include "windowcapture.h"
#include "imageprocessor.h"
#include <memory>

class WindowCaptureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WindowCaptureWidget(QWidget *parent = 0);


signals:

public slots:
    void paintEvent(QPaintEvent *e);
    void timerEvent(QTimerEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void setAlgorithm( int algo );

    void updateState( const QVariantMap& vmap );

private:

    void calcImageEMADiff( cv::Mat& img );
    void calcImageLaplacian( cv::Mat& img );

    WindowCapture* _cap;
    cv::Mat final_img;
    bool first;
    int counter;
    double avg_time;
    std::vector< std::shared_ptr<ImageProcessor> > _processor_list;
    std::shared_ptr<ImageProcessor> _processor;
};

