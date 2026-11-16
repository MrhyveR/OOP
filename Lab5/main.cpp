#define NOMINMAX
#include <windows.h>
#include <commctrl.h>
#include <cstdio>
#include "editor.h"
#include "table.h"
#include "resource.h"

MyTable tableWindow;

void UpdateWindowTitle(HWND hwnd) {
    MyEditor* editor = MyEditor::GetInstance();
    char title[256];
    sprintf(title, "Lab5 - Graphic Editor [Selected Tool: %s]", editor->GetCurrentTypeName());
    SetWindowTextA(hwnd, title);
}

HMENU CreateApplicationMenu() {
    HMENU hMenu = CreateMenu();

    HMENU hFileMenu = CreatePopupMenu();
    AppendMenuA(hFileMenu, MF_STRING, IDM_OPEN, "Open...");
    AppendMenuA(hFileMenu, MF_STRING, IDM_SAVE, "Save...");
    AppendMenuA(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuA(hFileMenu, MF_STRING, IDM_EXIT, "Exit");
    AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, "File");

    HMENU hObjMenu = CreatePopupMenu();
    AppendMenuA(hObjMenu, MF_STRING, IDM_POINT, "Point");
    AppendMenuA(hObjMenu, MF_STRING, IDM_LINE, "Line");
    AppendMenuA(hObjMenu, MF_STRING, IDM_RECT, "Rectangle");
    AppendMenuA(hObjMenu, MF_STRING, IDM_ELLIPSE, "Ellipse");
    AppendMenuA(hObjMenu, MF_STRING, IDM_LINECIRCLE, "Line with circles");
    AppendMenuA(hObjMenu, MF_STRING, IDM_CUBE, "Cube");
    AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hObjMenu, "Objects");

    HMENU hWinMenu = CreatePopupMenu();
    AppendMenuA(hWinMenu, MF_STRING, IDM_TABLE, "Object table");
    AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hWinMenu, "View");

    HMENU hHelpMenu = CreatePopupMenu();
    AppendMenuA(hHelpMenu, MF_STRING, IDM_ABOUT, "About");
    AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, "Help");

    return hMenu;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    MyEditor* editor = MyEditor::GetInstance();

    switch (uMsg) {
        case WM_CREATE: {
            InitCommonControls();
            HWND hToolBar = CreateWindowExA(0, TOOLBARCLASSNAMEA, NULL,
                WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | CCS_TOP,
                0, 0, 0, 0, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

            SendMessage(hToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
            SendMessage(hToolBar, TB_SETBITMAPSIZE, 0, MAKELPARAM(24, 24));

            HBITMAP hBitmap = (HBITMAP)LoadImageA(NULL, "res\\toolbar.bmp",
                IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

            HIMAGELIST hImages = ImageList_Create(24, 24, ILC_COLOR24 | ILC_MASK, 6, 0);
            if (hBitmap) {
                ImageList_AddMasked(hImages, hBitmap, RGB(192, 192, 192));
                DeleteObject(hBitmap);
            }

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
            editor->OnCommand(hwnd, wParam);
            if (LOWORD(wParam) >= IDM_POINT && LOWORD(wParam) <= IDM_CUBE) {
                UpdateWindowTitle(hwnd);
            }
            if (LOWORD(wParam) == IDM_TABLE) {
                tableWindow.Toggle(GetModuleHandle(NULL), hwnd);
            }
            if (LOWORD(wParam) == IDM_EXIT) {
                DestroyWindow(hwnd);
            }
            if (tableWindow.IsVisible()) {
                tableWindow.UpdateTable();
            }
            break;

        case WM_NOTIFY:
            return editor->OnNotify(lParam);

        case WM_LBUTTONDOWN:
            editor->OnLButtonDown(hwnd, lParam);
            break;

        case WM_MOUSEMOVE:
            editor->OnMouseMove(hwnd, lParam);
            break;

        case WM_LBUTTONUP:
            editor->OnLButtonUp(hwnd, lParam);
            if (tableWindow.IsVisible()) {
                tableWindow.UpdateTable();
            }
            break;

        case WM_PAINT:
            editor->OnPaint(hwnd);
            break;

        case WM_DESTROY:
            tableWindow.Hide();
            MyEditor::DestroyInstance();
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    const char CLASS_NAME[] = "ObjectEditorClass";

    WNDCLASSA wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "Lab5 - Graphic Editor",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, 900, 650,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 0;

    HMENU hMenu = CreateApplicationMenu();
    SetMenu(hwnd, hMenu);

    UpdateWindowTitle(hwnd);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return 0;
}