#include <windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // MessageBox(nullptr, L"hello", L"Test", MB_OK);

    WNDCLASSEX wc    = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"TestWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        L"TestWindow",
        L"XXXX",
        WS_OVERLAPPEDWINDOW,
        100, 100, 600, 400,
        nullptr, nullptr, hInstance, nullptr
    );
    SetWindowDisplayAffinity(hwnd, 0x00000011); // WDA_EXCLUDEFROMCAPTURE
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    HWND hwnd2 = CreateWindowEx(
        0,
        L"TestWindow",
        L"XXXX",
        WS_OVERLAPPEDWINDOW,
        100, 100, 600, 1200,
        nullptr, nullptr, hInstance, nullptr
    );
    SetWindowDisplayAffinity(hwnd2, 0x00000011);


    ShowWindow(hwnd2, nCmdShow);
    UpdateWindow(hwnd2);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

