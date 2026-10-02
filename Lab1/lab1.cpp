#define UNICODE
#define _UNICODE

#include <windows.h>
#include <wchar.h>
#include "module1.h"
#include "module2.h"

#define IDM_WORK1 3001
#define IDM_WORK2 3002

static wchar_t g_DisplayText[256] = L"No result selected";

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_WORK1: {
            wchar_t inputText[256];
            if (ShowModule1Dialog(hWnd, inputText, 256)) {
                swprintf(g_DisplayText, 256, L"Entered text: %ls", inputText);
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        }
        case IDM_WORK2: {
            int number = 0;
            if (ShowModule2Dialog(hWnd, number)) {
                swprintf(g_DisplayText, 256, L"Selected number: %d", number);
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        }
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        TextOutW(hdc, 20, 20, g_DisplayText, (int)wcslen(g_DisplayText));
        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"MainLab1Window";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);

    HMENU hMenu = CreateMenu();
    AppendMenuW(hMenu, MF_STRING, IDM_WORK1, L"Work1");
    AppendMenuW(hMenu, MF_STRING, IDM_WORK2, L"Work2");

    HWND hWnd = CreateWindowExW(
        0, CLASS_NAME, L"Lab1",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 450, 300,
        NULL, hMenu, hInstance, NULL
    );

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}