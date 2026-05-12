#include <windows.h>
#include <Mmdeviceapi.h>

#define INITGUID
#include <guiddef.h>
#include <mmdeviceapi.h>
#include <string>

#include <functiondiscoverykeys_devpkey.h>

enum class ID : int
{
    InputChat  = 101,
};

HWND hInput = nullptr; 
HWND mWindow = nullptr;

// general msg loop
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
                (HMENU)ID::InputChat,
                nullptr, nullptr
            );
        return 0;

    case WM_CLOSE:
        if ( mWindow && mWindow == hwnd ){
            DestroyWindow(hwnd);
        }
        return 0;
    
    case WM_COMMAND:
        if (LOWORD(wParam) == (int)ID::InputChat )
        {
            // ... keyboard inputs
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
    
    CoInitialize(nullptr);
    
    IMMDeviceEnumerator* pEnum = nullptr;
    auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnum);
    if (FAILED(hr)) return -1;

    IMMDeviceCollection* pCollection = nullptr; // &pCollection == **
    pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);

    UINT count = 0;
    pCollection->GetCount(&count);

    for (auto i = 0; i < count; i++)
    {
        IMMDevice* d = nullptr;
        pCollection->Item(i, &d);

        LPWSTR id = nullptr;
        d->GetId(&id);

        IPropertyStore* pProps = nullptr;
        d->OpenPropertyStore(STGM_READ, &pProps);
        PROPVARIANT name;
        PropVariantInit(&name);
        pProps->GetValue(PKEY_Device_FriendlyName, &name);
        MessageBox(nullptr, name.pwszVal, L"Device", MB_OK);

    
        PropVariantClear(&name); // win resources
        pProps->Release();

        MessageBox(nullptr, id, L"Device ID", MB_OK);

        CoTaskMemFree(id);  // free allocated by win
        d->Release();
    }

    

    std::wstring s = std::to_wstring(count);
    MessageBox(nullptr, s.c_str(),L"AUDIO DEVICES", MB_OK);
    
    pCollection->Release();
    pEnum->Release();
    CoUninitialize();

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
        L"XXXX0123",
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

