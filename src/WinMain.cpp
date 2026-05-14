#include <windows.h>
#include <Mmdeviceapi.h>
#include <winhttp.h>

#define INITGUID
#include <guiddef.h>
#include <mmdeviceapi.h>
#include <string>
#include <cstdint>

#include <functiondiscoverykeys_devpkey.h>

struct ResponseCtx {
    BYTE buffer[4096];
    std::string response;
};

enum class ID : int
{
    InputChat  = 101,
};

HWND hInput = nullptr; 
HWND mWindow = nullptr;


VOID CALLBACK HttpCb(
    HINTERNET hInternet,
    DWORD_PTR dwContext,
    DWORD dwInternetStatus, // event
    LPVOID lpvStatusInformation,
    DWORD dwStatusInformationLength
)
{
    ResponseCtx* ctx = reinterpret_cast<ResponseCtx*>(dwContext); //dwContext juste DWORD du ptr passé au debut juste là on recast dans le PTR pour revenir à l'objet initial de base pour continuer à remplir la string / reset le buffer

    switch (dwInternetStatus)
    {
    case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
        WinHttpReceiveResponse(hInternet, NULL);
        break;

    case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
        WinHttpReadData(hInternet, ctx->buffer, sizeof(ctx->buffer), NULL);
        break;

    case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
        if (dwStatusInformationLength > 0) {
            ctx->response.append((char*)ctx->buffer, dwStatusInformationLength);
            WinHttpReadData(hInternet, ctx->buffer, sizeof(ctx->buffer), NULL);
        } else {
            //printf("%s\n", ctx->response.c_str());
            MessageBoxA(nullptr, ctx->response.c_str(), "Réponse", MB_OK);
            delete ctx;
            WinHttpCloseHandle(hInternet);
        }
        break;
        
    case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
        delete ctx;
        WinHttpCloseHandle(hInternet);
        break;
    }
}


int32_t device() {
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

    return 0;
}

void setupWinHttpCb(HINTERNET hSession) {
    WinHttpSetStatusCallback(
        hSession,
        HttpCb,
        WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS,
        0
    );
}

// hwnd general msg loop
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

// winhttp cb
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
        L"45444",
        WS_OVERLAPPEDWINDOW,
        100, 100, 600, 400,
        nullptr, nullptr, hInstance, nullptr
    );
    SetWindowDisplayAffinity(mWindow, WDA_EXCLUDEFROMCAPTURE); // WDA_EXCLUDEFROMCAPTURE
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

    // https://learn.microsoft.com/fr-fr/windows/win32/winhttp/winhttp-sessions-overview
    HINTERNET hSession = WinHttpOpen(
        L"JustCheat/1.0",              
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,        
        WINHTTP_NO_PROXY_BYPASS,
        WINHTTP_FLAG_ASYNC
    );
    if (!hSession) {
        MessageBoxA(nullptr, "hSession est null !", "Erreur", MB_OK);
        return -1;
    }
    setupWinHttpCb(hSession);


    HINTERNET hConnect = WinHttpConnect(
        hSession,
        L"jsonplaceholder.typicode.com",
        INTERNET_DEFAULT_HTTPS_PORT,
        0
    );
    if (!hConnect) {
        MessageBoxA(nullptr, "hConnect est null !", "Erreur", MB_OK);
        return -1;
    }


    HINTERNET req = WinHttpOpenRequest(
        hConnect,
        L"GET",
        L"/todos/1",
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE
    );
    if (!req) {
        MessageBoxA(nullptr, "req est null !", "Erreur", MB_OK);
        return -1;
    }

    ResponseCtx* ctx = new ResponseCtx();
    BOOL ok = WinHttpSendRequest(
        req,
        WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, 0,
        (DWORD_PTR)ctx
    );
    if (!ok) {
        DWORD err = GetLastError();
        std::string msg = "SendRequest failed: " + std::to_string(err);
        MessageBoxA(nullptr, msg.c_str(), "Erreur", MB_OK);
    }

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    
    return 0;
}

