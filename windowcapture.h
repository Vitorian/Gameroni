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

#include <vector>
#include "win32utils.h"

class WindowCapture
{
private:
    const bool screenshot;
    CaptureState* state;

    void initialize();

    const HWND hWnd;
public:

    WindowCapture(HWND window, bool screenshot = false);
    ~WindowCapture();

    HWND handle() const { return hWnd; }
    int width();
    int height();
    void getMousePosition( int& x, int& y );
    void sendMouseMove( int x, int y );
    void sendMouseClick( int x, int y );
    void captureFrame(cv::Mat& dst);
};


