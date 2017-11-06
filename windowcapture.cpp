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

#include "windowcapture.h"
#include <iostream>


WindowCapture::WindowCapture(HWND window, bool screenshot)
    : hWnd(window), screenshot(screenshot), state(nullptr)
{ }

WindowCapture::~WindowCapture()
{
    if ( state )
        delete state;
}

void WindowCapture::initialize()
{
    if (state)
        return;

    if (screenshot)
        revealWindow(hWnd);

    state = new SnapshotCapture;
    state->startCapture( hWnd, screenshot );
}

int WindowCapture::width()
{
    initialize();
    return state->getWidth();
}

int WindowCapture::height()
{
    initialize();
    return state->getHeight();
}

void WindowCapture::captureFrame( cv::Mat& dst )
{
    initialize();
    state->captureFrame( dst );
}


void WindowCapture::getMousePosition( int& x, int& y )
{
    POINT pt;
    GetCursorPos(&pt);
    x = pt.x; //state->wr.left;
    y = pt.y; //state->wr.top;
}

void WindowCapture::sendMouseClick( int x, int y )
{
    int offsetX = 0; //state->cr.left;
    int offsetY = 0; //state->cr.top;
    DWORD dw = MAKEWORD( offsetX + x, offsetY + y );

    SendMessage(hWnd,WM_SETCURSOR,(int)hWnd,MAKELPARAM(HTCLIENT,WM_MOUSEMOVE));
    SendMessage(hWnd,WM_MOUSEMOVE,0,dw);
    //SendMessage(hWnd,WM_LBUTTONDOWN,MK_LBUTTON,dw);
    //SendMessage(hWnd,WM_LBUTTONUP,MK_LBUTTON,dw);
    ::SetCapture(hWnd);
    ::SendMessage(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, dw);
    ::SendMessage(hWnd, WM_LBUTTONUP, MK_LBUTTON, dw);
    ::ReleaseCapture();
}





