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

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <QVariantMap>
#include <QTime>

/** Base class for processing images
* Derive from this class in order to create a new processor
If you want to add a new image processor "XXX", the path is:
1. Add a new tab to MainWindow/CentralWidget/toolBox
2. Populate tab with the respective settings
3. Add a corresponding entry for serialization in MainWindow::updateState()
4. Implement a new class ImageProcessorXXX (files imageprocessorXXX.h and imageprocessorXXX.cpp)
*/
class ImageProcessor
{
public:
    ImageProcessor();

    /** Process the raw image and produces a final image that can be color or grayscale. */
    virtual void process( cv::Mat& raw_img, cv::Mat& final_img, int counter ) =0;
    virtual void updateState( const QVariantMap& vmap ) =0;
};
