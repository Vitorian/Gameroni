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

#include "imageprocessormotion.h"
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>

ImageProcessorMotion::ImageProcessorMotion()
{
    lambda = 1/30.;
    img_width = 1280;
    rigid_transform = false;
    reset_image = true;
    last_time = QTime::currentTime();
    use_color = true;
}

void ImageProcessorMotion::updateState( const QVariantMap& vmap )
{
    double halflife = vmap["Duration"].toDouble();
    int new_img_width  = vmap["Width"].toInt();
    lambda = log(2)/halflife;
    rigid_transform = vmap["RigidTransform"].toBool();
    bool grayscale = vmap["Grayscale"].toBool();
    if ( (((grayscale?1:0)^(use_color?1:0))!=0) || (img_width!=new_img_width) ) {
        reset_image = true;
    }
    use_color = !grayscale;
    img_width = new_img_width;
}

static std::string cvImageType(int imgTypeInt)
{

    int enum_ints[] =       {CV_8U,  CV_8UC1,  CV_8UC2,  CV_8UC3,  CV_8UC4,
                             CV_8S,  CV_8SC1,  CV_8SC2,  CV_8SC3,  CV_8SC4,
                             CV_16U, CV_16UC1, CV_16UC2, CV_16UC3, CV_16UC4,
                             CV_16S, CV_16SC1, CV_16SC2, CV_16SC3, CV_16SC4,
                             CV_32S, CV_32SC1, CV_32SC2, CV_32SC3, CV_32SC4,
                             CV_32F, CV_32FC1, CV_32FC2, CV_32FC3, CV_32FC4,
                             CV_64F, CV_64FC1, CV_64FC2, CV_64FC3, CV_64FC4};
    const int numImgTypes = sizeof(enum_ints)/sizeof(int); // 7 base types, with five channel options each (none or C1, ..., C4)

    std::string enum_strings[] = {"CV_8U",  "CV_8UC1",  "CV_8UC2",  "CV_8UC3",  "CV_8UC4",
                             "CV_8S",  "CV_8SC1",  "CV_8SC2",  "CV_8SC3",  "CV_8SC4",
                             "CV_16U", "CV_16UC1", "CV_16UC2", "CV_16UC3", "CV_16UC4",
                             "CV_16S", "CV_16SC1", "CV_16SC2", "CV_16SC3", "CV_16SC4",
                             "CV_32S", "CV_32SC1", "CV_32SC2", "CV_32SC3", "CV_32SC4",
                             "CV_32F", "CV_32FC1", "CV_32FC2", "CV_32FC3", "CV_32FC4",
                             "CV_64F", "CV_64FC1", "CV_64FC2", "CV_64FC3", "CV_64FC4"};

    for(int i=0; i<numImgTypes; i++)
    {
        if(imgTypeInt == enum_ints[i]) return enum_strings[i];
    }
    return "unknown image type";
}

static void printType( const std::string& name, cv::Mat& abs_img ) {
    std::cout << name << " Depth:" << abs_img.depth()
              << " ElSize:" << abs_img.elemSize()
              << " Size:" << abs_img.size[0] << "," << abs_img.size[1]
              << " Type:" << cvImageType(abs_img.type())
              << std::endl;
}

void ImageProcessorMotion::process( cv::Mat &raw_img,
                                    cv::Mat &final_img,
                                    int counter )
{
    /// Should we reset the image?
    /// Remember the flag can also be set in updateState()
    if ( counter==0 ) reset_image = true;

    /// Take time now
    QTime curtime = QTime::currentTime();

    /// Options
    bool resize = false;
    bool denoise = false;
    const int gauss_radius = 5;
    const int blur_radius = 11;

    /// De-noise image
    cv::Mat denoise_img;
    if ( denoise ) {
        GaussianBlur( raw_img, denoise_img, cv::Size(gauss_radius,gauss_radius),
                      0, 0, cv::BORDER_DEFAULT );
    }
    else {
        denoise_img = raw_img;
    }

    /// Resize (optional)
    cv::Mat sm_img;
    if ( resize ) {
        if ( reset_image ) {
            double aspect = double(img_width)/denoise_img.size().width;
            cv::resize( denoise_img, sm_img, cv::Size(0,0), aspect, aspect, cv::INTER_LINEAR );
        }
        else {
            cv::resize( denoise_img, sm_img, cv::Size(acc_img.cols,acc_img.rows), 0, 0, cv::INTER_LINEAR );
        }
    }
    else {
        sm_img = denoise_img;
    }

    /// Convert image to floating point so we can make calculations
    cv::Mat float_img;
    if ( use_color ) {
        /// Convert to float so we can average
        sm_img.convertTo( float_img, CV_32FC4 );
    }
    else {
        /// Convert the image to grayscale
        cv::Mat gray_img;
        cvtColor( sm_img, gray_img, CV_BGR2GRAY );

        /// Convert to a floating point representation
        gray_img.convertTo( float_img, CV_32FC1 );
    }

    /// Apply rigid transform
    cv::Mat blur_img;
    if ( rigid_transform ) {
        cvtColor( raw_img, blur_img, CV_BGR2GRAY );
        GaussianBlur( blur_img, blur_img, cv::Size(blur_radius,blur_radius),
                      0, 0, cv::BORDER_DEFAULT );

        bool warpok = (!reset_image) &&
                (blur_img.size()==last_img.size()) &&
                (blur_img.type()==last_img.type());
        if ( warpok ) {
            cv::Mat aff_mat = cv::estimateRigidTransform( blur_img, last_img, false );
            if ( !aff_mat.empty() ) {
                double sc = aff_mat.at<double>(0,0);
                double sxy = aff_mat.at<double>(0,1);
                double dx = aff_mat.at<double>(0,2);
                double dy = aff_mat.at<double>(1,2);
                std::cerr << sc << " " << sxy << " " << dx << " " << dy << std::endl;
                int width = blur_img.size[0];
                int height = blur_img.size[1];
                int maxlen = std::max(width,height);
                double maxdv = fabs((sc-1)*maxlen) + fabs(sxy*maxlen) + fabs(dx) + fabs(dy);
                if ( maxdv<1000 ) {
                    cv::Size dsize = float_img.size();
                    cv::warpAffine( float_img, float_img, aff_mat, dsize );
                }
                else {
                    std::cerr << "MaxDV:" << maxdv << " " << aff_mat << std::endl;
                    //cv::Size dsize = acc_img.size();
                    //cv::warpAffine( acc_img, acc_img, aff_mat, dsize );
                    acc_img = float_img.clone();
                    last_img = blur_img.clone();
                }
            }
            else {
                /// Could not calculate
                std::cerr << "Could not calculate - could not compute affine" << std::endl;
                acc_img = float_img.clone();
                last_img = blur_img.clone();
            }
        }
        else {
            std::cerr << "Could not calculate - image sizes are different" << std::endl;
            printType( "Last", last_img );
            printType( "Blur ", blur_img );
            acc_img = float_img.clone();
            last_img = blur_img.clone();
        }
    }
    else {
        blur_img = float_img;
    }


    /// accumulate image
    bool calcok = (!reset_image) &&
            (acc_img.size()==float_img.size()) &&
            (acc_img.type()==float_img.type());
    if ( calcok ) {
        double secs = last_time.msecsTo(curtime)/1000.;
        double factor = exp( -lambda*secs );
        acc_img = factor*acc_img + (1-factor)*float_img;
    }
    else {
        //printf( "Size: %d x %d\n", gray_sm_img.rows, gray_sm_img.cols );
        acc_img = float_img.clone();
    }


    /// Compute difference between image and background
    cv::Mat diff_img = float_img - acc_img;

    if ( use_color ) {
        cv::Mat abs_img;
        convertScaleAbs( diff_img, abs_img );
        abs_img.convertTo( final_img, CV_8UC3 );
    }
    else {
        cv::Mat gray8b_img;
        convertScaleAbs( diff_img, gray8b_img );
        cvtColor( gray8b_img, final_img, CV_GRAY2RGB );
    }

    last_time = curtime;
    reset_image = false;
}

