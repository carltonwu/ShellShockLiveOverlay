#include "Paint.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include <d2d1.h>
#include <d3d9.h>
#include <math.h>


int Paint::d3D9Init(HWND hWnd) {

    if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &d3dObject))) {
        exit(1);
    }

    ZeroMemory(&d3dparams, sizeof(d3dparams));

    d3dparams.BackBufferWidth = width;
    d3dparams.BackBufferHeight = height;
    d3dparams.Windowed = TRUE;
    d3dparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dparams.hDeviceWindow = hWnd;
    d3dparams.MultiSampleQuality = D3DMULTISAMPLE_NONE;
    d3dparams.BackBufferFormat = D3DFMT_A8R8G8B8;
    d3dparams.EnableAutoDepthStencil = TRUE;
    d3dparams.AutoDepthStencilFormat = D3DFMT_D16;


    HRESULT res = d3dObject->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dparams, 0, &d3dDevice);

    if (FAILED(res)) {
        //std::wstring ws(DXGetErrorString(res));
        //std::string str(ws.begin(), ws.end());
        //std::wstring ws2(DXGetErrorDescription(res));
        //std::string str2(ws2.begin(), ws2.end());
        //std::string error = "Error: " + str + " error description: " + str2;
        exit(1);
    }

    D3DXCreateFont(d3dDevice, 50, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEVICE_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Comic Sans", &d3dFont);

    return 0;

}

Paint::Paint() {};

Paint::Paint(HWND hWnd, HWND targetWnd, int width, int height) {
    this->width = width;
    this->height = height;
    this->targetWnd = targetWnd;
    this->hWnd = hWnd;
    d3D9Init(hWnd);
}

bool enabled = false;
bool ug = false;
bool dw = false;


int Paint::render(int power, int angle, float x, float y, int realAngle, float wind)
{

    //variables
    double pi = 3.141593;
    float tracerOffset = width * .013;
    float incrementValue = 0.05f;
    float uiCutoff = height * .85;
    float bumperSize = width * .005;

    if (d3dDevice == nullptr)
        return 1;
    d3dDevice->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
    d3dDevice->BeginScene();

    if (GetAsyncKeyState(VK_OEM_3) & 0x8000) enabled = true;
    if (GetAsyncKeyState(VK_OEM_3) & 0x8000 && GetAsyncKeyState(VK_SHIFT) & 0x8000) enabled = false;
    if (GetAsyncKeyState(0x31) & 0x8000) ug = true;
    if (GetAsyncKeyState(0x31) & 0x8000 && GetAsyncKeyState(VK_SHIFT) & 0x8000) ug = false;
    if (GetAsyncKeyState(0x32) & 0x8000) dw = true;
    if (GetAsyncKeyState(0x32) & 0x8000 && GetAsyncKeyState(VK_SHIFT) & 0x8000) dw = false;

    if (targetWnd == GetForegroundWindow())
    {

        //std::string numStr = std::to_string(width);
        //drawText((char*)&numStr, 200, height / 20, 255, 171, 255, 182);


        drawCircle(x, y);
        drawCircleFilled(x, y, 5, D3DCOLOR_ARGB(255, 225, 0, 0));

        if (y < uiCutoff) {
            double radians = angle * (pi / 180);

            int offsetX = tracerOffset * cos(abs(radians));
            int offsetY = tracerOffset * sin(abs(radians));
            float max1x, max2x;
            float max1y = height, max2y = height;
            float v = 1.561;
            float vx = 1.565;
            float wv = .0125;
            float g = 9.8;

            if (ug) {
                g = -1 * 9.8;
                vx = 1.544;
                v = 1.613;
                wv = 0.0117;
            }
            if (dw)
                wind = 2 * wind;

            // 2560x1440 float v = 1.615;
            // 2560x1440 float vx = 1.620;
            // 2560x1440 float wv = 0.0133;

            if (realAngle <= 90) {
                x = x + offsetX;
                y = y - offsetY;
            }
            else if (realAngle <= 180) {
                x = x + offsetX;
                y = y + offsetY;
            }
            else if (realAngle <= 270) {
                x = x - offsetX;
                y = y + offsetY;
            }
            else {
                x = x - offsetX;
                y = y - offsetY;
            }

            for (float i = 0; i <= 50; i += incrementValue) {

                if (realAngle < 180)
                {
                    float circleX = (x + power * vx * cos(radians) * i + 0.5 * wind * wv * pow(i, 2));
                    float circleY = (y - power * v * sin(radians) * i + 0.5 * g * pow(i, 2));
                    if (circleY < max1y && circleY < (y - power * v * sin(radians) * (i + incrementValue) + 0.5 * g * pow(i + incrementValue, 2))) {
                        max1y = circleY;
                        max1x = circleX;
                    }

                    if (circleY >= (uiCutoff * height));
                    else if (circleY <= 0);
                    else if (circleX <= 0);
                    else if (circleX >= width - bumperSize) {
                        x = circleX;
                        y = circleY;
                        float difference = circleY - (y - power * v * sin(radians) * i + 0.5 * g * pow(i, 2));

                        for (float c = 0; c <= 50; c += incrementValue) {
                            float circleX = (x - power * vx * cos(radians) * c + 0.5 * wind * wv * pow(c, 2));
                            float circleY = (y - power * v * sin(radians) * i + 0.5 * g * pow(i, 2)) + difference;
                            if (circleY < max1y && circleY < (y - power * v * sin(radians) * i + incrementValue + 0.5 * g * pow(i + incrementValue, 2)) + difference) {
                                max2y = circleY;
                                max2x = circleX;
                            }

                            if (circleX <= 0 + bumperSize) {
                                x = circleX;
                                y = circleY;
                                float difference = circleY - (y - power * v * sin(radians) * i + 0.5 * g * pow(i, 2));

                                for (float k = 0; k <= 50; k += incrementValue) {
                                    float circleX = (x + power * vx * cos(radians) * k + 0.5 * wind * wv * pow(k, 2));
                                    float circleY = (y - power * v * sin(radians) * i + 0.5 * g * pow(i, 2)) + difference;

                                    if (circleY < (uiCutoff * height))
                                        drawCircleFilled(circleX, circleY, 3, D3DCOLOR_ARGB(255, 70, 255, 55));

                                    i += incrementValue;
                                }
                                break;
                            }
                            else if (circleY < (uiCutoff * height))
                                drawCircleFilled(circleX, circleY, 3, D3DCOLOR_ARGB(200, 255, 120, 153));

                            i += incrementValue;
                        }
                        break;
                    }
                    else {
                        if (fmod(i, 2) != 0)
                            drawCircleFilled(circleX, circleY, 3, D3DCOLOR_ARGB(200, 101, 255, 180));
                    }

                }
                else {
                    float circleX = (x - power * vx * cos(radians) * i + 0.5 * wind * wv * pow(i, 2));
                    float circleY = (y - power * v * sin(radians) * i + 0.5 * g * pow(i, 2));
                    if (circleY < max1y && circleY < (y - power * 1.559 * sin(radians) * i + incrementValue + 0.5 * g * pow(i + incrementValue, 2))) {
                        max1y = circleY;
                        max1x = circleX;
                    }

                    if (circleY >= (uiCutoff * height));
                    else if (circleY <= 0);
                    else if (circleX <= 0 + bumperSize) {
                        x = circleX;
                        y = circleY;
                        float difference = circleY - (y - power * v * sin(radians) * i + 0.5 * g * pow(i, 2));

                        for (float c = 0; c <= 50; c += incrementValue) {
                            float circleX = (x + power * vx * cos(radians) * c + 0.5 * wind * wv * pow(c, 2));
                            float circleY = (y - power * v * sin(radians) * i + 0.5 * g * pow(i, 2)) + difference;
                            if (circleY < max1y && circleY < (y - power * v * sin(radians) * i + incrementValue + 0.5 * g * pow(i + incrementValue, 2)) + difference) {
                                max2y = circleY;
                                max2x = circleX;
                            }

                            if (circleX >= width - bumperSize) {
                                x = circleX;
                                y = circleY;
                                float difference = circleY - (y - power * v * sin(radians) * i + 0.5 * g * pow(i, 2));

                                for (float c = 0; c <= 50; c += incrementValue) {
                                    float circleX = (x - power * vx * cos(radians) * c + 0.5 * wind * wv * pow(c, 2));
                                    float circleY = (y - power * v * sin(radians) * i + 0.5 * g * pow(i, 2)) + difference;

                                    if (circleY < (uiCutoff * height))
                                        drawCircleFilled(circleX, circleY, 3, D3DCOLOR_ARGB(255, 70, 255, 55));

                                    i += incrementValue;
                                }
                                break;
                            }
                            else if (circleY < (uiCutoff * height))
                                drawCircleFilled(circleX, circleY, 3, D3DCOLOR_ARGB(200, 255, 120, 153));

                            i += incrementValue;
                        }
                        break;
                    }
                    else if (circleX >= width);
                    else {
                        if (fmod(i, 2) != 0)
                            drawCircleFilled(circleX, circleY, 3, D3DCOLOR_ARGB(200, 101, 255, 180));
                    }
                }
            }

            if (enabled) {

                for (float i = 0; i <= 50; i += incrementValue) {
                    float circleX = (max1x - 0 * vx * cos(radians) * i + 0.5 * wind * wv * pow(i, 2));
                    float circleY = (max1y - 0 * v * sin(radians) * i + 0.5 * g * pow(i, 2));
                    if (circleY < (uiCutoff * height)) {
                        drawCircleFilled(circleX, circleY, 2, D3DCOLOR_ARGB(255, 255, 0, 0));
                    }
                }
            }
        }
    }
    d3dDevice->EndScene();
    d3dDevice->PresentEx(0, 0, 0, 0, 0);

    return 0;
}

void Paint::drawText(char* string, int x, int y, int a, int r, int g, int b)
{
    RECT FontPos;
    FontPos.left = x;
    FontPos.top = y;
    d3dFont->DrawTextA(0, string, strlen(string), &FontPos, DT_NOCLIP, D3DCOLOR_ARGB(a, r, g, b));
}

void Paint::drawLine(float x1, float y1, float x2, float y2)
{
    LPD3DXLINE line;
    D3DXCreateLine(d3dDevice, &line);
    D3DXVECTOR2 lines[] = { D3DXVECTOR2(x1, y1), D3DXVECTOR2(x2, y2) };
    line->Begin();
    line->Draw(lines, 2, D3DCOLOR_ARGB(255, 255, 255, 255));
    line->End();
}

void Paint::drawCircle(float x, float y)
{
    LPD3DXLINE gLine;
    D3DXCreateLine(d3dDevice, &gLine);
    D3DXVECTOR2 Line[128];
    float Step = (float)3.1415 * 2.0 / 20;
    int Count = 0;
    for (float a = 0; a < (float)3.1415 * 2.0; a += Step)
    {
        float X1 = 7 * cos(a) + x;
        float Y1 = 7 * sin(a) + y;
        float X2 = 7 * cos(a + Step) + x;
        float Y2 = 7 * sin(a + Step) + y;
        Line[Count].x = X1;
        Line[Count].y = Y1;
        Line[Count + 1].x = X2;
        Line[Count + 1].y = Y2;
        Count += 2;
    }
    gLine->Begin();
    gLine->Draw(Line, Count, D3DCOLOR_ARGB(255, 225, 45, 255));
    gLine->End();
}

static const int CIRCLE_RESOLUTION = 16;

struct VERTEX_2D_DIF { // transformed colorized
    float x, y, z, rhw;
    D3DCOLOR color;
    static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
};

void Paint::drawCircleFilled(float mx, float my, float r, D3DCOLOR color)
{
    VERTEX_2D_DIF verts[CIRCLE_RESOLUTION + 1];

    for (int i = 0; i < CIRCLE_RESOLUTION + 1; i++)
    {
        verts[i].x = mx + r * cos(D3DX_PI * (i / (CIRCLE_RESOLUTION / 2.0f)));
        verts[i].y = my + r * sin(D3DX_PI * (i / (CIRCLE_RESOLUTION / 2.0f)));
        verts[i].z = 0;
        verts[i].rhw = 1;
        verts[i].color = color;
    }

    d3dDevice->SetFVF(VERTEX_2D_DIF::FVF);
    d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, CIRCLE_RESOLUTION - 1, &verts, sizeof(VERTEX_2D_DIF));
}
