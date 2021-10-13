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
#include <windows.h>
#include <dshow.h>
#include <QDebug>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "strmiids.lib")

HRESULT EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum)
{
    // Create the System Device Enumerator.
    ICreateDevEnum *pDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

    if (SUCCEEDED(hr))
    {
        // Create an enumerator for the category.
        hr = pDevEnum->CreateClassEnumerator(category, ppEnum, 0);
        if (hr == S_FALSE)
        {
            hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
        }
        pDevEnum->Release();
    }
    return hr;
}

void DisplayDeviceInformation(IEnumMoniker *pEnum)
{
    IMoniker *pMoniker = NULL;

    while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
    {
        IPropertyBag *pPropBag;
        HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
        if (FAILED(hr))
        {
            pMoniker->Release();
            continue;
        }

        VARIANT var;
        VariantInit(&var);

        // Get description or friendly name.
        hr = pPropBag->Read(L"Description", &var, 0);
        if (FAILED(hr))
        {
            hr = pPropBag->Read(L"FriendlyName", &var, 0);
        }
        if (SUCCEEDED(hr))
        {
            //printf("%S\n", var.bstrVal);
            qDebug() << "===== Device:" << QString::fromWCharArray(var.bstrVal);
            VariantClear(&var);
        }

        hr = pPropBag->Write(L"FriendlyName", &var);
        if ( FAILED(hr) ) {
            qDebug() << "Failed to write key FriendlyName: " << QString::fromWCharArray(var.bstrVal);
        }

        // WaveInID applies only to audio capture devices.
        hr = pPropBag->Read(L"WaveInID", &var, 0);
        if (SUCCEEDED(hr))
        {
            // printf("WaveIn ID: %d\n", var.lVal);
            qDebug() << "  WaveIn ID:" << var.lVal;
            VariantClear(&var);
        }

        hr = pPropBag->Read(L"DevicePath", &var, 0);
        if (SUCCEEDED(hr))
        {
            // The device path is not intended for display.
            //printf("Device path: %S\n", var.bstrVal);
            qDebug() << "  Device path:" << QString::fromWCharArray(var.bstrVal);
            VariantClear(&var);
        }

        pPropBag->Release();
        pMoniker->Release();
    }
}

void PrintDevices()
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED ); //COINIT_MULTITHREADED);
    if (SUCCEEDED(hr))
    {
        IEnumMoniker *pEnum;

        hr = EnumerateDevices(CLSID_VideoInputDeviceCategory, &pEnum);
        if (SUCCEEDED(hr))
        {
            DisplayDeviceInformation(pEnum);
            pEnum->Release();
        }
        /*hr = EnumerateDevices(CLSID_AudioInputDeviceCategory, &pEnum);
        if (SUCCEEDED(hr))
        {
            DisplayDeviceInformation(pEnum);
            pEnum->Release();
        }*/
        CoUninitialize();
    }
    else {
        qDebug() << "Failed to initialize COM library. Error code 0x" << QString( "%1" ).arg(hr,0,16);
    }
}

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

        DWORD namelen = GetProcessImageFileNameW( process, nameBuffer.data(), (DWORD) nameBuffer.size() );
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

        DWORD namelen = GetProcessImageFileNameW(process, nameBuffer.data(), (DWORD)nameBuffer.size() );
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
    return ( ((a.left >= b.left) && (a.left < b.right)) || ((a.right > b.left) && (a.right <= b.right)) )
        && ( ((a.top >= b.top) && (a.top < b.bottom)) || ((a.bottom > b.top) && (a.bottom <= b.bottom)) );
}


void revealWindow(HWND window)
{
    if (!IsWindow(window))
        throw std::invalid_argument("Window does not exist.");

    if (IsIconic(window))
        SendMessage(window, WM_SYSCOMMAND, SC_RESTORE, 0);

    RECT cr;
    GetClientRect(window, &cr);

    POINT clientOffset = { 0, 0 };
    if (!ClientToScreen(window, &clientOffset))
        throw std::invalid_argument("Could not get client area offset.");

    moveRect(cr, clientOffset.x, clientOffset.y);

    // Find the HWND of the window after which to insert the current window
    // in z-order. This is the window in front of the topmost window occluding
    // the current window (if any).
    bool targetNext = false;
    HWND target = 0;
    HWND next = window;
    while ( (next = GetWindow(next, GW_HWNDPREV)) != NULL)
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
