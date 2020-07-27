// ShellShockLiveOverlay.cpp : Defines the entry point for the application.
//

#include <Dwmapi.h>
#include "framework.h"
#include "ShellShockLiveOverlay.h"
#include "Paint.h"
#include <Windows.h>
#include <iostream>
#include <vector>
#include <TlHelp32.h>
#include <tchar.h>

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR overlayWindowName[100] = L"Overlay";  // main window class name & The title bar text
LPCSTR targetWindowName = "ShellShock Live";  // main window class name & The title bar text
HWND targetHWND, overlayHWND;
int width, height;
Paint paint;

int main();
void updateValues();

HANDLE pHandle;

DWORD powerAddress;
DWORD angleAddress;
DWORD windAddress;

int currentPower = 0;
int currentAngle = 0;
float currentWind = 0;
int realAngle = 0;
long x = 0;
long y = 0;

// Forward declarations of functions included in this code module:
ATOM                registerClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    registerClass(hInstance);


    targetHWND = FindWindowA(0, targetWindowName);
    if (targetHWND) {
        RECT rect;
        GetWindowRect(targetHWND, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;;
        main();
    }
    else
        return FALSE;

    // Perform application initialization:
    if (!InitInstance(hInstance, SW_SHOW)) {
        return FALSE;
    }
    paint = Paint(overlayHWND, targetHWND, width, height);
    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0)) {

        TranslateMessage(&msg);
        DispatchMessage(&msg);

        RECT rect;
        GetWindowRect(targetHWND, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
        MoveWindow(overlayHWND, rect.left, rect.top, width, height, true);

        updateValues();
    }

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM registerClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(nullptr, IDC_CROSS);
    wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wcex.lpszMenuName = overlayWindowName;
    wcex.lpszClassName = overlayWindowName;
    wcex.hIconSm = 0;

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    overlayHWND = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, overlayWindowName, overlayWindowName, WS_POPUP,
        1, 1, width, height, nullptr, nullptr, hInstance, nullptr);
    SetLayeredWindowAttributes(overlayHWND, 0, 1.0f, LWA_ALPHA);
    SetLayeredWindowAttributes(overlayHWND, 0, RGB(0, 0, 0), LWA_COLORKEY);

    if (!overlayHWND) {
        return FALSE;
    }

    ShowWindow(overlayHWND, nCmdShow);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT:
        paint.render(currentPower, currentAngle, x - (width * .0011), y + (width * .0011), realAngle, currentWind * 100);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

DWORD dwGetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID) {
    DWORD dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
    MODULEENTRY32 ModuleEntry32 = { 0 };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32))
    {
        do {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0)
            {
                dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32));

    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}


int main() {
    DWORD pID;
    DWORD poff1, poff2, poff3, poff4, poff5, poff6, poff7;
    DWORD aoff1, aoff2, aoff3, aoff4, aoff5, aoff6, aoff7;
    DWORD woff1, woff2, woff3, woff4, woff5, woff6, woff7;
    DWORD baseAddress;
    DWORD windBaseAddress;
    char moduleName[] = "UnityPlayer.dll";
    HWND hGameWindow;


    //Get Handles
    hGameWindow = FindWindow(NULL, "ShellShock Live");
    GetWindowThreadProcessId(hGameWindow, &pID);
    pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);

    //Get Client Base Addy
    DWORD clientBase = dwGetModuleBaseAddress(_T(moduleName), pID);
}

void updateValues() {
    DWORD pID;
    DWORD poff1, poff2, poff3, poff4, poff5, poff6, poff7, poff8;
    DWORD aoff1, aoff2, aoff3, aoff4, aoff5, aoff6, aoff7, aoff8;
    DWORD woff1, woff2, woff3, woff4, woff5, woff6, woff7, woff8;
    DWORD powerBaseAddress;
    DWORD angleBaseAddress;
    DWORD windBaseAddress;
    char moduleName[] = "UnityPlayer.dll";
    char moduleName2[] = "mono.dll";
    HWND hGameWindow;


    //Get Handles
    hGameWindow = FindWindow(NULL, "ShellShock Live");
    GetWindowThreadProcessId(hGameWindow, &pID);
    pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);

    //Get Client Base Addy
    DWORD clientUnityBase = dwGetModuleBaseAddress(_T(moduleName), pID);
    DWORD clientMonoBase = dwGetModuleBaseAddress(_T(moduleName2), pID);

    ReadProcessMemory(pHandle, (LPCVOID)(clientUnityBase + 0x00FE2984), &powerBaseAddress, sizeof(powerBaseAddress), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(powerBaseAddress + 0xEC), &poff1, sizeof(poff1), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(poff1 + 0xC4), &poff2, sizeof(poff2), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(poff2 + 0x2B4), &poff3, sizeof(poff3), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(poff3 + 0x854), &poff4, sizeof(poff4), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(poff4 + 0xA8), &poff5, sizeof(poff5), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(poff5 + 0x4), &poff6, sizeof(poff6), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(poff6 + 0xC), &poff7, sizeof(poff7), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(poff7 + 0x0), &poff8, sizeof(poff8), NULL);
    powerAddress = poff8 + 0x20;

    ReadProcessMemory(pHandle, (LPCVOID)(clientMonoBase + 0x001F72E8), &angleBaseAddress, sizeof(angleBaseAddress), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(angleBaseAddress + 0x3C), &aoff1, sizeof(aoff1), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(aoff1 + 0x20), &aoff2, sizeof(aoff2), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(aoff2 + 0x7E8), &aoff3, sizeof(aoff3), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(aoff3 + 0x20), &aoff4, sizeof(aoff4), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(aoff4 + 0x1C), &aoff5, sizeof(aoff5), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(aoff5 + 0x2C), &aoff6, sizeof(aoff6), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(aoff6 + 0x74), &aoff7, sizeof(aoff7), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(aoff7 + 0x8), &aoff8, sizeof(aoff8), NULL);
    angleAddress = aoff8 + 0x8C;

    ReadProcessMemory(pHandle, (LPCVOID)(clientUnityBase + 0x00FA2418), &windBaseAddress, sizeof(windBaseAddress), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(windBaseAddress + 0x0), &woff1, sizeof(woff1), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(woff1 + 0x18), &woff2, sizeof(woff2), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(woff2 + 0x1C), &woff3, sizeof(woff3), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(woff3 + 0x4C), &woff4, sizeof(woff4), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(woff4 + 0x8), &woff5, sizeof(woff5), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(woff5 + 0x1C), &woff6, sizeof(woff6), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(woff6 + 0xC), &woff7, sizeof(woff7), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(woff7 + 0x18), &woff8, sizeof(woff8), NULL);
    windAddress = woff8 + 0x14;

    ReadProcessMemory(pHandle, (LPCVOID)(powerAddress), &currentPower, sizeof(currentPower), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(powerAddress + -4), &currentAngle, sizeof(currentAngle), NULL);
    ReadProcessMemory(pHandle, (LPCVOID)(windAddress), &currentWind, sizeof(currentWind), NULL);

    if (currentAngle < 0) {
        currentAngle = 360 - abs(currentAngle);
    }

    realAngle = currentAngle;

    if (currentAngle <= 90) {
        currentAngle = 90 - currentAngle;
    }
    else if (currentAngle > 90 && currentAngle <= 180) {
        currentAngle = 0 - (currentAngle - 90);
    }
    else if (currentAngle > 180 && currentAngle <= 270) {
        currentAngle = 0 - (270 - currentAngle);
    }
    else if (currentAngle > 270) {
        currentAngle = currentAngle - 270;
    }

    if (GetAsyncKeyState(VK_CONTROL) && 0x45)
    {
        POINT point;
        GetCursorPos(&point);
        x = point.x + (long)(width * .0051);
        y = point.y + (long)(height * .0051);
    }

}