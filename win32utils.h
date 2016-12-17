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

#include <string>
#include <windows.h>
#include <opencv/cv.h>

struct CaptureState
{
    HWND window;
    int width, height;
    RECT wr, cr;

    bool screenshot;
    HDC windowDc;
    HDC memDc;
    HBITMAP hbmp;
    BITMAPINFOHEADER bi;
};

CaptureState* beginCaptureWindow(HWND window, bool screenshot = false);
void endCaptureWindow(CaptureState*& cs);
void captureFrame(const CaptureState* cs, cv::Mat& dst);

void captureWindow(HWND window, cv::Mat& dst, bool screenshot = false);

void getHwndsByProcessId(DWORD processId, std::vector<HWND>& list, bool visibleOnly = true);
void getHwndsByProcessName(const wchar_t* processName, std::vector<HWND>& list, bool visibleOnly = true);

struct ProcessInfo {
    std::wstring name;
    std::wstring process;
    DWORD processId;
    HWND  hWnd;
};

void getProcessList( std::vector< ProcessInfo >& proclist, bool visibleOnly );

void revealWindow(HWND window);

