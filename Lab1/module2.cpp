#define UNICODE
#define _UNICODE

#include "module2.h"
#include <commctrl.h>
#include <wchar.h>

static int g_ScrollPos = 50;
static bool g_IsOk2 = false;

static void UpdateValText(HWND hWnd) {
    wchar_t buf[32];
    swprintf(buf, 32, L"Value: %d", g_ScrollPos);
    SetDlgItemTextW(hWnd, 1002, buf);
}

static LRESULT CALLBACK Mod2Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        INITCOMMONCONTROLSEX icex = { sizeof(INITCOMMONCONTROLSEX), ICC_BAR_CLASSES };
        InitCommonControlsEx(&icex);

        CreateWindowExW(0, L"STATIC", L"Select a number from 1 to 100:", WS_CHILD | WS_VISIBLE,
                        20, 15, 250, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);

        HWND hTrack = CreateWindowExW(0, TRACKBAR_CLASSW, L"", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
                                      20, 40, 250, 30, hWnd, (HMENU)1001, GetModuleHandle(NULL), NULL);
        SendMessage(hTrack, TBM_SETRANGE, TRUE, MAKELONG(1, 100));
        SendMessage(hTrack, TBM_SETPOS, TRUE, g_ScrollPos);

        CreateWindowExW(0, L"STATIC", L"Value: 50", WS_CHILD | WS_VISIBLE,
                        20, 75, 150, 20, hWnd, (HMENU)1002, GetModuleHandle(NULL), NULL);

        CreateWindowExW(0, L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                        40, 105, 90, 28, hWnd, (HMENU)IDOK, GetModuleHandle(NULL), NULL);
        CreateWindowExW(0, L"BUTTON", L"Cancel", WS_CHILD | WS_VISIBLE,
                        160, 105, 90, 28, hWnd, (HMENU)IDCANCEL, GetModuleHandle(NULL), NULL);
        break;
    }
    case WM_HSCROLL: {
        HWND hTrack = GetDlgItem(hWnd, 1001);
        if (hTrack) {
            g_ScrollPos = (int)SendMessage(hTrack, TBM_GETPOS, 0, 0);
            UpdateValText(hWnd);
        }
        break;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            g_IsOk2 = true;
            DestroyWindow(hWnd);
        } else if (LOWORD(wParam) == IDCANCEL) {
            g_IsOk2 = false;
            DestroyWindow(hWnd);
        }
        break;
    case WM_CLOSE:
        g_IsOk2 = false;
        DestroyWindow(hWnd);
        break;
    default:
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

bool ShowModule2Dialog(HWND hWndParent, int& outValue) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    const wchar_t CLASS_NAME[] = L"Mod2ScrollDlgClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = Mod2Proc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    RegisterClassW(&wc);

    g_IsOk2 = false;
    g_ScrollPos = 50;

    HWND hDlg = CreateWindowExW(WS_EX_DLGMODALFRAME, CLASS_NAME, L"Work 2 - Scrollbar (1-100)",
                                WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                                350, 300, 300, 180, hWndParent, NULL, hInst, NULL);

    EnableWindow(hWndParent, FALSE);
    MSG msg;
    while (IsWindow(hDlg) && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    EnableWindow(hWndParent, TRUE);
    SetForegroundWindow(hWndParent);

    if (g_IsOk2) {
        outValue = g_ScrollPos;
        return true;
    }
    return false;
}