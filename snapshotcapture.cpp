#include "snapshotcapture.h"
#include "win32utils.h"
#include <psapi.h>
#include <opencv/cv.h>
#include <QDebug>

SnapshotCapture::SnapshotCapture()
{
    initialized = false;
    width = height = 0;
}

int  SnapshotCapture::getWidth()
{
    return width;
}

int  SnapshotCapture::getHeight()
{
    return height;
}


bool SnapshotCapture::startCapture(HWND hwnd, bool is_screenshot)
{
    if (!IsWindow(hwnd)) {
        qDebug() << "SnapshotCapture::startCapture(): Not a window";
        return false;
    }
    if ( initialized )
        endCapture();
    window = hwnd;
    screenshot = is_screenshot;

    try
    {
        windowDc = GetWindowDC(screenshot ? 0 : window);
        memDc = CreateCompatibleDC(windowDc);

        if (!windowDc || !memDc) {
            qDebug() << "SnapshotCapture::startCapture(): Could not create WindowDC" << windowDc << "or memDc" << memDc;
            return false;
        }

        GetWindowRect(window, &wr);
        GetClientRect(window, &cr);

        POINT clientOffset = { 0, 0 };
        if (!ClientToScreen(window, &clientOffset)) {
            qDebug() << "SnapshotCapture::startCapture(): Could not convert client to screen coordinates" << clientOffset.x << "," << clientOffset.y;
            return false;
        }

        moveRect(cr, clientOffset.x, clientOffset.y);

        width = cr.right - cr.left;
        height = cr.bottom - cr.top;

        if (width == 0 || height == 0) {
            qDebug() << "SnapshotCapture::startCapture(): Width" << width << "or height" << height << "are zero";
            return false;
        }

        hbmp = CreateCompatibleBitmap(windowDc, width, height);

        if (!hbmp) {
            qDebug() << "SnapshotCapture::startCapture(): Could not create compatible bitmap";
            return false;
        }

        SelectObject(memDc, hbmp);

        bi =
        {
            sizeof(BITMAPINFOHEADER),   // biSize
            width,                  // biWidth
            -height,                // biHeight
            1,                          // biPlanes
            32,                         // biBitCount
            BI_RGB,                     // biCompression
            0,                          // biSizeImage
            0,                          // biXPelsPerMeter
            0,                          // biYPelsPerMeter
            0,                          // biClrUser
            0                           // biClrImportant
        };

        initialized = true;
        return true;
    }
    catch (std::exception& e)
    {
        qDebug() << "SnapshotCapture::startCapture(): Exception thrown " << e.what();
        endCapture();
        throw e;
    }
}

void SnapshotCapture::endCapture()
{
    if ( initialized ) {
        DeleteObject(hbmp);
        DeleteObject(memDc);
        ReleaseDC(screenshot ? 0 : window, windowDc);
        initialized = false;
    }
}

bool SnapshotCapture::captureFrame(cv::Mat& dst)
{
    if ( !initialized ) {
        //qDebug() << "SnapshotCapture::captureFrame() : not initialized";
        return false;
    }

    int offsetX = cr.left;
    int offsetY = cr.top;
    if (!screenshot) {
        offsetX -= wr.left;
        offsetY -= wr.top;
    }

    if (!BitBlt(memDc, 0, 0, width, height, windowDc, offsetX, offsetY, SRCCOPY)) {
         qDebug() << "SnapshotCapture::captureFrame() : BitBlt failed";
         return false;
    }

    dst.create(height, width, CV_8UC4);

    if (!dst.data) {
        qDebug() << "SnapshotCapture::captureFrame() : could not allocate destination matrix";
        throw std::invalid_argument("Could not allocate destination matrix.");
    }

    int success = GetDIBits(memDc, hbmp, 0, (UINT)height, dst.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    if (!success || success == ERROR_INVALID_PARAMETER) {
        qDebug() << "SnapshotCapture::captureFrame() : GetDIBits failed";
        return false;
    }

    return true;
}

