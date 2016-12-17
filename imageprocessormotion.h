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

#include "imageprocessor.h"

class ImageProcessorMotion : public ImageProcessor
{
public:
    ImageProcessorMotion();
    virtual void process( cv::Mat& raw_img, cv::Mat& final_img, int counter ) override;
    virtual void updateState( const QVariantMap& vmap );
private:
    double lambda;
    int img_width;
    cv::Mat acc_img;
    cv::Mat last_img;
    QTime   last_time;
    bool rigid_transform;
    bool use_color;
    bool reset_image;
};

