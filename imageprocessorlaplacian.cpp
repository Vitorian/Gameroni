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

#include "imageprocessorlaplacian.h"
#include <opencv/cv.h>
#include <opencv2/videoio.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

ImageProcessorLaplacian::ImageProcessorLaplacian()
{
    radius = 3;
    kernel = 3;
}


void ImageProcessorLaplacian::updateState( const QVariantMap& vmap )
{
    radius = vmap["Radius"].toInt();
    kernel = vmap["Kernel"].toInt();
}

void ImageProcessorLaplacian::process(cv::Mat &raw_img, cv::Mat &final_img, int /*counter*/ )
{
    /// Remove noise by blurring with a Gaussian filter
    int eff_radius = radius%2==0 ? radius+1 : radius;
    GaussianBlur( raw_img, raw_img, cv::Size(eff_radius,eff_radius), 0, 0, cv::BORDER_DEFAULT );

    /// Convert the image to grayscale
    cv::Mat gray_img;
    cvtColor( raw_img, gray_img, CV_BGR2GRAY );

    /// Apply Laplace function
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;
    cv::Mat lap_img;
    int eff_kernel = kernel%2==0 ? kernel+1 : kernel;
    Laplacian( gray_img, lap_img, ddepth, eff_kernel, scale, delta, cv::BORDER_DEFAULT );
    convertScaleAbs( lap_img, lap_img );
    cvtColor( lap_img, final_img, CV_GRAY2RGB );
}
