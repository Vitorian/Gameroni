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

#include "win32utils.h"
#include <psapi.h>
#include <opencv/cv.h>

struct GetHwndsState
{
    // Input parameters
    DWORD processId;        // used by getHwndsByProcessId
    const wchar_t* winText; // getHwndsByWindowText
    // Results
    std::vector<HWND>& list;
    bool visibleOnly;
};

bool endsWith( const wchar_t* nameBuffer, const wchar_t* processName ) {
    return wcsstr(nameBuffer, processName) - nameBuffer == wcslen(nameBuffer) - wcslen(processName);
}

static BOOL CALLBACK testProcessIdCallback(HWND hWnd, LPARAM lParam)
{
    GetHwndsState* state = (GetHwndsState*)lParam;

    if (state->visibleOnly && !(GetWindowLong(hWnd, GWL_STYLE) & WS_VISIBLE))
        return true;

    DWORD processId;
    GetWindowThreadProcessId(hWnd, &processId);

    if (processId == state->processId)
        state->list.push_back(hWnd);

    return true;
}


static BOOL CALLBACK testWindowTextCallback(HWND hWnd, LPARAM lParam)
{
    GetHwndsState* state = (GetHwndsState*)lParam;

    if (state->visibleOnly && !(GetWindowLong(hWnd, GWL_STYLE) & WS_VISIBLE))
        return true;

    std::vector<wchar_t> wname(1024);
    GetWindowText( hWnd, wname.data(), (int)wname.size() );

    wchar_t* res = wcsstr(wname.data(), state->winText);
    if ( res!=NULL )
        state->list.push_back(hWnd);

    return true;
}

void getHwndsByWindowText( const wchar_t* text, std::vector<HWND>& list, bool visibleOnly)
{
    GetHwndsState state = { 0, text, list, visibleOnly };
    EnumWindows( testWindowTextCallback, (LPARAM)&state);
}

void getHwndsByProcessId(DWORD processId, std::vector<HWND>& list, bool visibleOnly)
{
    GetHwndsState state = { processId, NULL, list, visibleOnly };
    EnumWindows( testProcessIdCallback, (LPARAM)&state);
}

void getProcessList( std::vector< ProcessInfo >& proclist, bool visibleOnly )
{
    std::vector<DWORD> pids( 1024 );
    DWORD bytesReturned;

    EnumProcesses( &pids[0], (DWORD)pids.size() * sizeof(DWORD), &bytesReturned);

    int processes = bytesReturned / sizeof(DWORD);
    std::vector<wchar_t> nameBuffer( 1024 );

    for (int i = 0; i < processes; i++)
    {
        HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, false, pids[i]);
        if (!process)
            continue;

        DWORD namelen = GetProcessImageFileNameW( process, nameBuffer.data(), nameBuffer.size() );
        if ( namelen>0 ) {
                std::vector<HWND> wlist;
                getHwndsByProcessId( pids[i], wlist, visibleOnly);
                if ( !wlist.empty() ) {
                    HWND hWnd = wlist[0];
                    std::vector<wchar_t> wname(1024);
                    int titlen = GetWindowText( hWnd, wname.data(), (int)wname.size() );
                    if ( titlen>0 ) {
                        ProcessInfo info;
                        info.hWnd = hWnd;
                        info.name = std::wstring( wname.data(), titlen );
                        info.process = std::wstring( nameBuffer.data(), namelen );
                        info.processId = pids[i];
                        proclist.push_back( info );
                        //::wprintf( L"%ld %s %s %d", (unsigned long)hWnd, info.name.c_str(), info.process.c_str(), info.processId );
                    }
                }
        }

        CloseHandle(process);
    }
}

void getHwndsByProcessName(const wchar_t* processName, std::vector<HWND>& list, bool visibleOnly)
{
    std::vector<DWORD> pids( 1024 );
    DWORD bytesReturned;

    EnumProcesses( &pids[0], (DWORD) pids.size() * sizeof(DWORD), &bytesReturned);

    int processes = bytesReturned / sizeof(DWORD);
    std::vector<wchar_t> nameBuffer( 1024 );

    for (int i = 0; i < processes; i++)
    {
        HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, false, pids[i]);
        if (!process)
            continue;

        DWORD namelen = GetProcessImageFileNameW(process, nameBuffer.data(), nameBuffer.size() );
        if ( namelen>0 ) {

            wprintf( L"Name:%s\n", nameBuffer.data() );
            if ( endsWith( nameBuffer.data(), processName ) ) {
                getHwndsByProcessId(GetProcessId(process), list, visibleOnly);
            }
        }

        CloseHandle(process);
    }

}

void moveRect(RECT& rect, int x, int y)
{
    rect.left += x, rect.right += x;
    rect.top += y, rect.bottom += y;
}

bool overlapRect(const RECT& a, const RECT& b)
{
    return (a.left >= b.left && a.left < b.right || a.right > b.left && a.right <= b.right)
        && (a.top >= b.top && a.top < b.bottom || a.bottom > b.top && a.bottom <= b.bottom);
}

void revealWindow(HWND window)
{
    if (!IsWindow(window))
        throw std::exception("Window does not exist.");

    if (IsIconic(window))
        SendMessage(window, WM_SYSCOMMAND, SC_RESTORE, 0);

    RECT cr;
    GetClientRect(window, &cr);

    POINT clientOffset = { 0, 0 };
    if (!ClientToScreen(window, &clientOffset))
        throw std::exception("Could not get client area offset.");

    moveRect(cr, clientOffset.x, clientOffset.y);

    // Find the HWND of the window after which to insert the current window
    // in z-order. This is the window in front of the topmost window occluding
    // the current window (if any).
    bool targetNext = false;
    HWND target = 0;
    HWND next = window;
    while (next = GetWindow(next, GW_HWNDPREV))
    {
        RECT wr;
        GetWindowRect(next, &wr);

        if (overlapRect(wr, cr))
            targetNext = true;
        else if (targetNext)
            target = next, targetNext = false;
    }

    if (targetNext)
        SetForegroundWindow(window);
    else if (target)
        SetWindowPos(window, target, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

CaptureState* beginCaptureWindow(HWND window, bool screenshot)
{
    if (!IsWindow(window))
        throw std::exception("Window does not exist.");

    CaptureState* cs = new CaptureState();

    cs->window = window;
    cs->screenshot = screenshot;

    try
    {
        cs->windowDc = GetWindowDC(screenshot ? 0 : window);
        cs->memDc = CreateCompatibleDC(cs->windowDc);

        if (!cs->windowDc || !cs->memDc)
            throw std::exception("Could not get the required device contexts.");

        GetWindowRect(window, &cs->wr);
        GetClientRect(window, &cs->cr);

        POINT clientOffset = { 0, 0 };
        if (!ClientToScreen(window, &clientOffset))
            throw std::exception("Could not get client area offset.");

        moveRect(cs->cr, clientOffset.x, clientOffset.y);

        cs->width = cs->cr.right - cs->cr.left;
        cs->height = cs->cr.bottom - cs->cr.top;

        if (cs->width == 0 || cs->height == 0)
            throw std::exception("Client area width or height is zero.");

        cs->hbmp = CreateCompatibleBitmap(cs->windowDc, cs->width, cs->height);

        if (!cs->hbmp)
            throw std::exception("Could not create bitmap.");

        SelectObject(cs->memDc, cs->hbmp);

        BITMAPINFOHEADER bi =
        {
            sizeof(BITMAPINFOHEADER),   // biSize
            cs->width,                  // biWidth
            -cs->height,                // biHeight
            1,                          // biPlanes
            32,                         // biBitCount
            BI_RGB,                     // biCompression
            0,                          // biSizeImage
            0,                          // biXPelsPerMeter
            0,                          // biYPelsPerMeter
            0,                          // biClrUser
            0                           // biClrImportant
        };
        cs->bi = bi;

        return cs;
    }
    catch (std::exception& e)
    {
        endCaptureWindow(cs);
        throw e;
    }
}

void endCaptureWindow(CaptureState*& cs)
{
    if (!cs)
        return;

    DeleteObject(cs->hbmp);
    DeleteObject(cs->memDc);
    ReleaseDC(cs->screenshot ? 0 : cs->window, cs->windowDc);

    delete cs;
    cs = 0;
}

void captureFrame(const CaptureState* cs, cv::Mat& dst)
{
    if (!cs)
        throw std::exception("Capture state is null.");

    int offsetX = cs->cr.left;
    int offsetY = cs->cr.top;
    if (!cs->screenshot)
        offsetX -= cs->wr.left, offsetY -= cs->wr.top;

    if (!BitBlt(cs->memDc, 0, 0, cs->width, cs->height, cs->windowDc, offsetX, offsetY, SRCCOPY))
        throw std::exception("Copying between device contexts failed.");

    dst.create(cs->height, cs->width, CV_8UC4);

    if (!dst.data)
        throw std::exception("Could not allocate destination matrix.");

    int success = GetDIBits(cs->memDc, cs->hbmp, 0, (UINT)cs->height, dst.data, (BITMAPINFO*)&cs->bi, DIB_RGB_COLORS);

    if (!success || success == ERROR_INVALID_PARAMETER)
        throw std::exception("Could not copy bitmap to matrix.");
}

void captureWindow(HWND window, cv::Mat& dst, bool screenshot)
{
    CaptureState* cs = beginCaptureWindow(window, screenshot);
    try
    {
        captureFrame(cs, dst);
        endCaptureWindow(cs);
    }
    catch (std::exception& e)
    {
        endCaptureWindow(cs);
        throw e;
    }
}
