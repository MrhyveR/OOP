#define NOMINMAX
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "editor.h"
#include "resource.h"
#include <cmath>
#include <algorithm>

MyEditor::MyEditor() : pcshape(nullptr), shapeCount(0), capacity(0), currentType(IDM_POINT), isDrawing(false) {
    ptStart.x = ptStart.y = 0;
    ptEnd.x = ptEnd.y = 0;
}

MyEditor::~MyEditor() {
    for (int i = 0; i < shapeCount; i++) {
        delete pcshape[i];
    }
    delete[] pcshape;
}

void MyEditor::EnsureCapacity() {
    if (capacity == 0) {
        capacity = 8;
        pcshape = new Shape*[capacity];
    } else if (shapeCount >= capacity) {
        capacity *= 2;
        Shape** newArr = new Shape*[capacity];
        for (int i = 0; i < shapeCount; i++) {
            newArr[i] = pcshape[i];
        }
        delete[] pcshape;
        pcshape = newArr;
    }
}

void MyEditor::SetCurrentType(int type) {
    currentType = type;
}

int MyEditor::GetCurrentType() const {
    return currentType;
}

void MyEditor::OnCommand(HWND hwnd, WPARAM wParam) {
    int wmId = LOWORD(wParam);
    switch (wmId) {
        case IDM_POINT:
        case IDM_LINE:
        case IDM_RECT:
        case IDM_ELLIPSE:
        case IDM_LINECIRCLE:
        case IDM_CUBE: {
            SetCurrentType(wmId);

            const wchar_t* toolName = L"";
            switch (wmId) {
                case IDM_POINT:        toolName = L"Точка"; break;
                case IDM_LINE:         toolName = L"Лінія"; break;
                case IDM_RECT:         toolName = L"Прямокутник"; break;
                case IDM_ELLIPSE:      toolName = L"Еліпс"; break;
                case IDM_LINECIRCLE:   toolName = L"Лінія з кружечками"; break;
                case IDM_CUBE:         toolName = L"Каркас куба"; break;
            }

            wchar_t titleBuf[256];
            wsprintfW(titleBuf, L"Редактор Обрано: %s", toolName);
            SetWindowTextW(hwnd, titleBuf);
            break;
        }
        case IDM_ABOUT: {
            MessageBoxW(hwnd,
                L"Лабораторна робота №4\nВаріант №4",
                L"Про програму",
                MB_OK | MB_ICONINFORMATION);
            break;
        }
        case IDM_EXIT:
            DestroyWindow(hwnd);
            break;
    }
}

LRESULT MyEditor::OnNotify(LPARAM lParam) {
    LPNMHDR pnmh = (LPNMHDR)lParam;
    if (pnmh->code == TTN_GETDISPINFOW) {
        LPNMTTDISPINFOW p = (LPNMTTDISPINFOW)lParam;
        const wchar_t* text = L"";

        switch (p->hdr.idFrom) {
            case IDM_POINT:        text = L"Точка"; break;
            case IDM_LINE:         text = L"Лінія"; break;
            case IDM_RECT:         text = L"Прямокутник"; break;
            case IDM_ELLIPSE:      text = L"Еліпс"; break;
            case IDM_LINECIRCLE:   text = L"Лінія з кружечками"; break;
            case IDM_CUBE:         text = L"Каркас куба"; break;
        }

        p->lpszText = (LPWSTR)text;
    }
    return 0;
}

void MyEditor::OnLButtonDown(HWND hwnd, LPARAM lParam) {
    isDrawing = true;
    ptStart.x = GET_X_LPARAM(lParam);
    ptStart.y = GET_Y_LPARAM(lParam);
    ptEnd = ptStart;
    SetCapture(hwnd);
}

void MyEditor::OnMouseMove(HWND hwnd, LPARAM lParam) {
    if (!isDrawing) return;

    ptEnd.x = GET_X_LPARAM(lParam);
    ptEnd.y = GET_Y_LPARAM(lParam);

    //FALSE означає не очищувати екран заздалегідь білим кольором (уникнемо миготіння та пропадіння)
    InvalidateRect(hwnd, NULL, FALSE);
}

void MyEditor::OnLButtonUp(HWND hwnd, LPARAM lParam) {
    if (!isDrawing) return;
    isDrawing = false;
    ReleaseCapture();

    ptEnd.x = GET_X_LPARAM(lParam);
    ptEnd.y = GET_Y_LPARAM(lParam);

    Shape* newShape = nullptr;
    switch (currentType) {
        case IDM_POINT:
            newShape = new PointShape(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
            break;
        case IDM_LINE:
            newShape = new LineShape(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
            break;
        case IDM_RECT:
            newShape = new RectangleShape(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
            break;
        case IDM_ELLIPSE:
            newShape = new EllipseShape(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
            break;
        case IDM_LINECIRCLE:
            newShape = new LineWithCirclesShape(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
            break;
        case IDM_CUBE:
            newShape = new CubeShape(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
            break;
    }

    if (newShape) {
        EnsureCapacity();
        pcshape[shapeCount++] = newShape;
    }

    InvalidateRect(hwnd, NULL, TRUE);
}

void MyEditor::OnPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT rc;
    GetClientRect(hwnd, &rc);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    //Білий фон тільки в буфері
    HBRUSH hWhiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
    FillRect(hdcMem, &rc, hWhiteBrush);

    //Усі збережені фігури
    for (int i = 0; i < shapeCount; i++) {
        pcshape[i]->Show(hdcMem);
    }

    //Гумовий слід
    if (isDrawing) {
        DrawRubberTrace(hdcMem, ptStart, ptEnd);
    }

    //Готовий кадр на екран
    BitBlt(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);

    EndPaint(hwnd, &ps);
}

void MyEditor::DrawRubberTrace(HDC hdc, POINT p1, POINT p2) {
    HPEN hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 255)); //Синій пунктир
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

    int oldBk = SetBkMode(hdc, TRANSPARENT);

    switch (currentType) {
        case IDM_POINT: {
            MoveToEx(hdc, p2.x - 5, p2.y, NULL);
            LineTo(hdc, p2.x + 5, p2.y);
            MoveToEx(hdc, p2.x, p2.y - 5, NULL);
            LineTo(hdc, p2.x, p2.y + 5);
            break;
        }
        case IDM_LINE: {
            MoveToEx(hdc, p1.x, p1.y, NULL);
            LineTo(hdc, p2.x, p2.y);
            break;
        }
        case IDM_RECT: {
            Rectangle(hdc, std::min(p1.x, p2.x), std::min(p1.y, p2.y),
                           std::max(p1.x, p2.x), std::max(p1.y, p2.y));
            break;
        }
        case IDM_ELLIPSE: {
            Ellipse(hdc, std::min(p1.x, p2.x), std::min(p1.y, p2.y),
                         std::max(p1.x, p2.x), std::max(p1.y, p2.y));
            break;
        }
        case IDM_LINECIRCLE: {
            int r = 10;
            long dx = p2.x - p1.x;
            long dy = p2.y - p1.y;
            double length = std::sqrt((double)dx * dx + (double)dy * dy);

            if (length > r) {
                long startX = p1.x + (long)(r * dx / length);
                long startY = p1.y + (long)(r * dy / length);
                long endX = p2.x - (long)(r * dx / length);
                long endY = p2.y - (long)(r * dy / length);

                MoveToEx(hdc, startX, startY, NULL);
                LineTo(hdc, endX, endY);
            }

            Ellipse(hdc, p1.x - r, p1.y - r, p1.x + r, p1.y + r);
            Ellipse(hdc, p2.x - r, p2.y - r, p2.x + r, p2.y + r);
            break;
        }
        case IDM_CUBE: {
            long x_min = std::min(p1.x, p2.x);
            long y_min = std::min(p1.y, p2.y);
            long x_max = std::max(p1.x, p2.x);
            long y_max = std::max(p1.y, p2.y);

            long w = x_max - x_min;
            long h = y_max - y_min;
            long offset = std::min(w, h) / 3;
            if (offset < 5) offset = 5;

            RECT front = { x_min,          y_min + offset, x_max - offset, y_max };
            RECT back  = { x_min + offset, y_min,          x_max,          y_max - offset };

            //Передня грань
            MoveToEx(hdc, front.left,  front.top,    NULL); LineTo(hdc, front.right, front.top);
            MoveToEx(hdc, front.right, front.top,    NULL); LineTo(hdc, front.right, front.bottom);
            MoveToEx(hdc, front.right, front.bottom, NULL); LineTo(hdc, front.left,  front.bottom);
            MoveToEx(hdc, front.left,  front.bottom, NULL); LineTo(hdc, front.left,  front.top);

            //Задня грань
            MoveToEx(hdc, back.left,  back.top,    NULL); LineTo(hdc, back.right, back.top);
            MoveToEx(hdc, back.right, back.top,    NULL); LineTo(hdc, back.right, back.bottom);
            MoveToEx(hdc, back.right, back.bottom, NULL); LineTo(hdc, back.left,  back.bottom);
            MoveToEx(hdc, back.left,  back.bottom, NULL); LineTo(hdc, back.left,  back.top);

            //4 ребра між гранями
            MoveToEx(hdc, front.left,  front.top,    NULL); LineTo(hdc, back.left,  back.top);
            MoveToEx(hdc, front.right, front.top,    NULL); LineTo(hdc, back.right, back.top);
            MoveToEx(hdc, front.left,  front.bottom, NULL); LineTo(hdc, back.left,  back.bottom);
            MoveToEx(hdc, front.right, front.bottom, NULL); LineTo(hdc, back.right, back.bottom);
            break;
        }
    }

    SetBkMode(hdc, oldBk);
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
}