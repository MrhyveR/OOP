#define UNICODE
#define _UNICODE

#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "editor.h"

#pragma comment(lib, "comctl32.lib")

//Динамічний об'єкт класу MyEditor
MyEditor* g_pEditor = nullptr;

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        //Динамічне створення об'єкта класу MyEditor
        g_pEditor = new MyEditor();

        InitCommonControls();
        HWND hToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
            WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | CCS_TOP,
            0, 0, 0, 0, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

        SendMessage(hToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
        SendMessage(hToolBar, TB_SETBITMAPSIZE, 0, MAKELPARAM(24, 24));

        HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, L"res\\toolbar.bmp",
            IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

        if (!hBitmap) {
            MessageBox(hwnd, L"Не вдалося завантажити res\\toolbar.bmp!", L"Помилка", MB_OK | MB_ICONERROR);
        }

        // 6 кнопок у тулбарі
        HIMAGELIST hImages = ImageList_Create(24, 24, ILC_COLOR24 | ILC_MASK, 6, 0);
        ImageList_AddMasked(hImages, hBitmap, RGB(192, 192, 192));
        DeleteObject(hBitmap);

        SendMessage(hToolBar, TB_SETIMAGELIST, 0, (LPARAM)hImages);

        TBBUTTON tbb[6];
        ZeroMemory(tbb, sizeof(tbb));

        tbb[0].iBitmap = 0; tbb[0].idCommand = IDM_POINT;      tbb[0].fsState = TBSTATE_ENABLED; tbb[0].fsStyle = TBSTYLE_BUTTON;
        tbb[1].iBitmap = 1; tbb[1].idCommand = IDM_LINE;       tbb[1].fsState = TBSTATE_ENABLED; tbb[1].fsStyle = TBSTYLE_BUTTON;
        tbb[2].iBitmap = 2; tbb[2].idCommand = IDM_RECT;       tbb[2].fsState = TBSTATE_ENABLED; tbb[2].fsStyle = TBSTYLE_BUTTON;
        tbb[3].iBitmap = 3; tbb[3].idCommand = IDM_ELLIPSE;    tbb[3].fsState = TBSTATE_ENABLED; tbb[3].fsStyle = TBSTYLE_BUTTON;
        tbb[4].iBitmap = 4; tbb[4].idCommand = IDM_LINECIRCLE; tbb[4].fsState = TBSTATE_ENABLED; tbb[4].fsStyle = TBSTYLE_BUTTON;
        tbb[5].iBitmap = 5; tbb[5].idCommand = IDM_CUBE;       tbb[5].fsState = TBSTATE_ENABLED; tbb[5].fsStyle = TBSTYLE_BUTTON;

        SendMessage(hToolBar, TB_ADDBUTTONS, (WPARAM)6, (LPARAM)&tbb);
        break;
    }
    case WM_COMMAND:
        if (g_pEditor) g_pEditor->OnCommand(hwnd, wParam);
        break;

    case WM_NOTIFY:
        if (g_pEditor) return g_pEditor->OnNotify(lParam);
        break;

    case WM_LBUTTONDOWN:
        if (g_pEditor) g_pEditor->OnLButtonDown(hwnd, lParam);
        break;

    case WM_MOUSEMOVE:
        if (g_pEditor) g_pEditor->OnMouseMove(hwnd, lParam);
        break;

    case WM_LBUTTONUP:
        if (g_pEditor) g_pEditor->OnLButtonUp(hwnd, lParam);
        break;

    case WM_PAINT:
        if (g_pEditor) g_pEditor->OnPaint(hwnd);
        break;

    case WM_DESTROY:
        //Коректне знищення динамічного об'єкта класу MyEditor
        delete g_pEditor;
        g_pEditor = nullptr;
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"classLab4";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreateMenu();
    HMENU hObjMenu = CreateMenu();
    HMENU hHelpMenu = CreateMenu();

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"Файл");
    AppendMenu(hFileMenu, MF_STRING, IDM_EXIT, L"Вихід");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hObjMenu, L"Об'єкти");
    AppendMenu(hObjMenu, MF_STRING, IDM_POINT, L"Крапка");
    AppendMenu(hObjMenu, MF_STRING, IDM_LINE, L"Лінія");
    AppendMenu(hObjMenu, MF_STRING, IDM_RECT, L"Прямокутник");
    AppendMenu(hObjMenu, MF_STRING, IDM_ELLIPSE, L"Еліпс");
    AppendMenu(hObjMenu, MF_STRING, IDM_LINECIRCLE, L"Лінія з кружечками");
    AppendMenu(hObjMenu, MF_STRING, IDM_CUBE, L"Каркас куба");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"Довідка");
    AppendMenu(hHelpMenu, MF_STRING, IDM_ABOUT, L"Про програму");

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, L"Лабораторна робота №4 - Редактор об'єктів",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, hMenu, hInstance, NULL
    );

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}