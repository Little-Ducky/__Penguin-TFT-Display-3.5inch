#include "Application.h"
#include "resource.h"

#define WM_TRAYICON (WM_APP + 1)

Application::~Application()
{
    Shell_NotifyIcon(NIM_DELETE, &m_nid);

    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
}

LRESULT CALLBACK Application::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Application* app = reinterpret_cast<Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (msg)
    {
    case WM_CREATE:
    {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        app = (Application*)cs->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);
        break;
    }

    case WM_TRAYICON:
    {
        if (lParam == WM_RBUTTONUP)
        {
            POINT pt;
            GetCursorPos(&pt);

            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING, 1, L"Закрыть");

            SetForegroundWindow(hwnd);

            TrackPopupMenu(
                hMenu,
                TPM_RIGHTBUTTON | TPM_BOTTOMALIGN,
                pt.x,
                pt.y,
                0,
                hwnd,
                NULL
            );

            DestroyMenu(hMenu);
            PostMessage(hwnd, WM_NULL, 0, 0);
        }
        break;
    }

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == 1)
        {
            DestroyWindow(hwnd);
        }
        break;
    }

    case WM_DESTROY:
    {
        Shell_NotifyIcon(NIM_DELETE, &app->m_nid);
        PostQuitMessage(0);
        break;
    }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool Application::init(HINSTANCE hInstance)
{
    const wchar_t CLASS_NAME[] = L"HiddenTrayApp";

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = &Application::WndProc;
    wc.lpszClassName = CLASS_NAME;

    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
    wc.hIconSm = wc.hIcon;

    if (!RegisterClassEx(&wc))
        return false;

    m_hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        CLASS_NAME,
        L"",
        WS_POPUP,
        0, 0, 0, 0,
        nullptr,
        nullptr,
        hInstance,
        this
    );

    if (!m_hwnd)
        return false;

    ShowWindow(m_hwnd, SW_HIDE);

    m_nid = {};
    m_nid.cbSize = sizeof(NOTIFYICONDATA);
    m_nid.hWnd = m_hwnd;
    m_nid.uID = 1;
    m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nid.uCallbackMessage = WM_TRAYICON;
    m_nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));

    wcscpy_s(m_nid.szTip, L"__Penguin!");

    Shell_NotifyIcon(NIM_ADD, &m_nid);

    return true;
}