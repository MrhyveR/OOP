#define NOMINMAX
#define UNICODE
#define _UNICODE

#include "editor.h"

ShapeObjectsEditor::ShapeObjectsEditor() : shapeCount(0), currentType(IDM_POINT), isDrawing(false) {
    ptStart = { 0, 0 };
    ptEnd = { 0, 0 };
    for (int i = 0; i < 105; i++) pcshape[i] = nullptr;
}

ShapeObjectsEditor::~ShapeObjectsEditor() {
    for (int i = 0; i < shapeCount; i++) {
        delete pcshape[i];
    }
}

void ShapeObjectsEditor::DrawRubberTrace(HWND hwnd, POINT p1, POINT p2) {
    HDC hdc = GetDC(hwnd);
    int oldRop = SetROP2(hdc, R2_NOTXORPEN);
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0)); // Червоний слід
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

    switch (currentType) {
    case IDM_POINT:
        SetPixel(hdc, p2.x, p2.y, RGB(255, 0, 0));
        break;
    case IDM_LINE:
        MoveToEx(hdc, p1.x, p1.y, NULL);
        LineTo(hdc, p2.x, p2.y);
        break;
    case IDM_RECT: {
        long left = p1.x - std::abs(p2.x - p1.x);
        long right = p1.x + std::abs(p2.x - p1.x);
        long top = p1.y - std::abs(p2.y - p1.y);
        long bottom = p1.y + std::abs(p2.y - p1.y);
        Rectangle(hdc, left, top, right, bottom);
        break;
    }
    case IDM_ELLIPSE:
        Ellipse(hdc, std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::max(p1.x, p2.x), std::max(p1.y, p2.y));
        break;
    }

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    SetROP2(hdc, oldRop);
    ReleaseDC(hwnd, hdc);
}

void ShapeObjectsEditor::OnCommand(HWND hwnd, WPARAM wParam) {
    int id = LOWORD(wParam);
    switch (id) {
    case IDM_POINT:
    case IDM_LINE:
    case IDM_RECT:
    case IDM_ELLIPSE:
        currentType = id;
        break;
    case IDM_EXIT:
        PostQuitMessage(0);
        break;
    case IDM_ABOUT:
        MessageBox(hwnd, L"Лабораторна робота №3", L"Про програму", MB_OK);
        break;
    }

    wchar_t title[120];
    if (currentType == IDM_POINT) wcscpy_s(title, 120, L"Редактор - Крапка");
    else if (currentType == IDM_LINE) wcscpy_s(title, 120, L"Редактор - Лінія");
    else if (currentType == IDM_RECT) wcscpy_s(title, 120, L"Редактор - Прямокутник");
    else if (currentType == IDM_ELLIPSE) wcscpy_s(title, 120, L"Редактор - Еліпс");
    SetWindowText(hwnd, title);
}

LRESULT ShapeObjectsEditor::OnNotify(LPARAM lParam) {
    LPNMHDR pnmh = (LPNMHDR)lParam;
    if (pnmh->code == TTN_GETDISPINFO) {
        LPTOOLTIPTEXT pttt = (LPTOOLTIPTEXT)lParam;
        switch (pttt->hdr.idFrom) {
        case IDM_POINT: wcscpy_s(pttt->szText, 80, L"Крапка"); break;
        case IDM_LINE: wcscpy_s(pttt->szText, 80, L"Лінія"); break;
        case IDM_RECT: wcscpy_s(pttt->szText, 80, L"Прямокутник"); break;
        case IDM_ELLIPSE: wcscpy_s(pttt->szText, 80, L"Еліпс"); break;
        }
    }
    return 0;
}

void ShapeObjectsEditor::OnLButtonDown(HWND hwnd, LPARAM lParam) {
    isDrawing = true;
    ptStart.x = LOWORD(lParam);
    ptStart.y = HIWORD(lParam);
    ptEnd = ptStart;
    SetCapture(hwnd);
}

void ShapeObjectsEditor::OnMouseMove(HWND hwnd, LPARAM lParam) {
    if (!isDrawing) return;
    DrawRubberTrace(hwnd, ptStart, ptEnd);
    ptEnd.x = LOWORD(lParam);
    ptEnd.y = HIWORD(lParam);
    DrawRubberTrace(hwnd, ptStart, ptEnd);
}

void ShapeObjectsEditor::OnLButtonUp(HWND hwnd, LPARAM lParam) {
    if (!isDrawing) return;

    DrawRubberTrace(hwnd, ptStart, ptEnd);

    ptEnd.x = LOWORD(lParam);
    ptEnd.y = HIWORD(lParam);

    isDrawing = false;
    ReleaseCapture();

    if (shapeCount < 105) {
        if (currentType == IDM_POINT)
            pcshape[shapeCount++] = new PointShape(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
        else if (currentType == IDM_LINE)
            pcshape[shapeCount++] = new LineShape(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
        else if (currentType == IDM_RECT)
            pcshape[shapeCount++] = new RectangleShape(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
        else if (currentType == IDM_ELLIPSE)
            pcshape[shapeCount++] = new EllipseShape(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);

        InvalidateRect(hwnd, NULL, TRUE);
    }
}

void ShapeObjectsEditor::OnPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    for (int i = 0; i < shapeCount; i++) {
        if (pcshape[i]) {
            pcshape[i]->Show(hdc);
        }
    }
    EndPaint(hwnd, &ps);
}