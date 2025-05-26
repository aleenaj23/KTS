#include <windows.h>
#include <math.h>

#define TIMER_ID 1
#define PI 3.14159265

double angle = 0.0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"MultiOrbitWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, L"Multi-Orbit Planet Simulation",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600, NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    SetTimer(hwnd, TIMER_ID, 30, NULL);  // 30ms = ~33 FPS

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static int width = 800, height = 600;

    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Double-buffering setup
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

        // Clear background
        HBRUSH bgBrush = CreateSolidBrush(RGB(255, 255, 255));
        RECT fullRect = { 0, 0, width, height };
        FillRect(memDC, &fullRect, bgBrush);
        DeleteObject(bgBrush);

        int cx = width / 2;
        int cy = height / 2;

        // Pens
        HPEN innerPen = CreatePen(PS_SOLID, 2, RGB(0, 200, 0));   // Green orbit
        HPEN outerPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));   // Blue orbit
        HPEN centerPen = CreatePen(PS_SOLID, 1, RGB(255, 215, 0));// Yellow sun outline

        // Brushes
        HBRUSH sunBrush = CreateSolidBrush(RGB(255, 255, 0));      // Yellow sun
        HBRUSH planet1Brush = CreateSolidBrush(RGB(255, 0, 255));  // Magenta
        HBRUSH planet2Brush = CreateSolidBrush(RGB(0, 200, 255));  // Cyan

        // Save original objects
        HPEN oldPen = (HPEN)SelectObject(memDC, innerPen);
        HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, GetStockObject(NULL_BRUSH));

        // Draw inner orbit
        SelectObject(memDC, innerPen);
        Ellipse(memDC, cx - 100, cy - 100, cx + 100, cy + 100);

        // Draw outer orbit
        SelectObject(memDC, outerPen);
        Ellipse(memDC, cx - 150, cy - 150, cx + 150, cy + 150);

        // Draw sun
        SelectObject(memDC, centerPen);
        SelectObject(memDC, sunBrush);
        Ellipse(memDC, cx - 15, cy - 15, cx + 15, cy + 15);

        // Calculate planet positions
        int x1 = (int)(cx + 100 * cos(angle));
        int y1 = (int)(cy + 100 * sin(angle));

        int x2 = (int)(cx + 150 * cos(angle + PI));
        int y2 = (int)(cy + 150 * sin(angle + PI));

        // Draw planets
        SelectObject(memDC, planet1Brush);
        Ellipse(memDC, x1 - 10, y1 - 10, x1 + 10, y1 + 10);

        SelectObject(memDC, planet2Brush);
        Ellipse(memDC, x2 - 10, y2 - 10, x2 + 10, y2 + 10);

        // Transfer buffer to screen
        BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

        // Cleanup
        SelectObject(memDC, oldPen);
        SelectObject(memDC, oldBrush);
        SelectObject(memDC, oldBitmap);

        DeleteObject(innerPen);
        DeleteObject(outerPen);
        DeleteObject(centerPen);
        DeleteObject(sunBrush);
        DeleteObject(planet1Brush);
        DeleteObject(planet2Brush);
        DeleteObject(memBitmap);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_TIMER:
        angle += 0.05;
        if (angle >= 2 * PI) angle -= 2 * PI;
        InvalidateRect(hwnd, NULL, FALSE);  // Avoid flicker
        return 0;

    case WM_SIZE:
        width = LOWORD(lParam);
        height = HIWORD(lParam);
        return 0;

    case WM_DESTROY:
        KillTimer(hwnd, TIMER_ID);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
