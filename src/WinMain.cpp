#include <windows.h>


HWND hInput = nullptr; 
HWND mWindow = nullptr;

// TODO Add ID check on case for HWND 
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {

    case WM_CREATE:
        hInput = CreateWindowEx(
                WS_EX_CLIENTEDGE,      
                L"EDIT",                
                L"",                    
                WS_CHILD | WS_VISIBLE,  
                10, 10,                 
                400, 30,                
                hwnd,                   
                (HMENU)101,
                nullptr, nullptr
            );
        return 0;

    case WM_CLOSE:
        if ( mWindow && mWindow == hwnd ){
            DestroyWindow(hwnd);
        }
        return 0;
        
    case WM_DESTROY:
        PostQuitMessage(0); // leave msg loop
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

    mWindow = CreateWindowEx(
        0,
        L"TestWindow",
        L"XXXX",
        WS_OVERLAPPEDWINDOW,
        100, 100, 600, 400,
        nullptr, nullptr, hInstance, nullptr
    );
    SetWindowDisplayAffinity(mWindow, 0x00000011); // WDA_EXCLUDEFROMCAPTURE
    ShowWindow(mWindow, nCmdShow);
    UpdateWindow(mWindow);

    // HWND hwnd2 = CreateWindowEx(
    //     0,
    //     L"TestWindow",
    //     L"XXXX",
    //     WS_OVERLAPPEDWINDOW,
    //     100, 100, 600, 1200,
    //     nullptr, nullptr, hInstance, nullptr
    // );
    // SetWindowDisplayAffinity(hwnd2, 0x00000011);


    // ShowWindow(hwnd2, nCmdShow);
    // UpdateWindow(hwnd2);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

