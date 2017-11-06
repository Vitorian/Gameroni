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

#include <windows.h>
#include <opencv/cv.h>

class SnapshotCapture
{
public:
    SnapshotCapture();
    bool startCapture(HWND window, bool screenshot = false );
    void endCapture();
    bool captureFrame( cv::Mat& dst );
    void revealWindow(HWND window);
    int  getWidth();
    int  getHeight();
private:
    HWND window;
    int width, height;
    RECT wr, cr;

    HDC windowDc;
    HDC memDc;
    HBITMAP hbmp;
    BITMAPINFOHEADER bi;

    bool screenshot;
    bool initialized;
};

