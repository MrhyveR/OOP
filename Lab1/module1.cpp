#define UNICODE
#define _UNICODE

#include "module1.h"
#include <wchar.h>

static wchar_t g_TempText[256] = L"";
static bool g_IsOk1 = false;

static LRESULT CALLBACK Mod1Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        CreateWindowExW(0, L"STATIC", L"Enter a line of text:", WS_CHILD | WS_VISIBLE,
                        20, 15, 240, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);

        CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                        20, 40, 240, 25, hWnd, (HMENU)1001, GetModuleHandle(NULL), NULL);

        CreateWindowExW(0, L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                        40, 80, 90, 28, hWnd, (HMENU)IDOK, GetModuleHandle(NULL), NULL);

        CreateWindowExW(0, L"BUTTON", L"Cancel", WS_CHILD | WS_VISIBLE,
                        150, 80, 90, 28, hWnd, (HMENU)IDCANCEL, GetModuleHandle(NULL), NULL);
        break;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            GetDlgItemTextW(hWnd, 1001, g_TempText, 256);
            g_IsOk1 = true;
            DestroyWindow(hWnd);
        } else if (LOWORD(wParam) == IDCANCEL) {
            g_IsOk1 = false;
            DestroyWindow(hWnd);
        }
        break;
    case WM_CLOSE:
        g_IsOk1 = false;
        DestroyWindow(hWnd);
        break;
    default:
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

bool ShowModule1Dialog(HWND hWndParent, wchar_t* outText, DWORD maxLen) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    const wchar_t CLASS_NAME[] = L"Mod1EditDlgClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = Mod1Proc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    RegisterClassW(&wc);

    g_IsOk1 = false;
    g_TempText[0] = L'\0';

    HWND hDlg = CreateWindowExW(WS_EX_DLGMODALFRAME, CLASS_NAME, L"Work 1 - Text Input",
                                WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                                350, 300, 290, 155, hWndParent, NULL, hInst, NULL);

    EnableWindow(hWndParent, FALSE);
    MSG msg;
    while (IsWindow(hDlg) && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    EnableWindow(hWndParent, TRUE);
    SetForegroundWindow(hWndParent);

    if (g_IsOk1) {
        wcsncpy(outText, g_TempText, maxLen - 1);
        outText[maxLen - 1] = L'\0';
        return true;
    }
    return false;
}