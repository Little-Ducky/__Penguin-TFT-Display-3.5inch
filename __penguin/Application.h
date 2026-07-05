#pragma once

#include <windows.h>

#include "resource.h"

class Application
{
public:
    bool init(HINSTANCE hInstance);
    ~Application();

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND m_hwnd = nullptr;
    NOTIFYICONDATA m_nid = {};
};