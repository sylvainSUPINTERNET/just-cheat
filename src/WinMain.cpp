#include <windows.h>
#include <print>
#include <ostream>
#include <format>
#include <iostream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    // std::cin.get();
    MessageBox(nullptr, L"hello", L"Test", MB_OK);
   
    return 0;
}

