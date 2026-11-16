#include "shape.h"
#include <algorithm>

void PointShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 1, isSelected ? RGB(0, 0, 255) : RGB(0, 0, 0));
    HGDIOBJ oldPen = SelectObject(hdc, hPen);
    SetPixel(hdc, x1, y1, isSelected ? RGB(0, 0, 255) : RGB(0, 0, 0));
    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

void LineShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, isSelected ? 2 : 1, isSelected ? RGB(0, 0, 255) : RGB(0, 0, 0));
    HGDIOBJ oldPen = SelectObject(hdc, hPen);
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

void RectangleShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, isSelected ? 2 : 1, isSelected ? RGB(0, 0, 255) : RGB(0, 0, 0));
    HBRUSH hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH); //Прямокутник із білим заповненням
    HGDIOBJ oldPen = SelectObject(hdc, hPen);
    HGDIOBJ oldBrush = SelectObject(hdc, hBrush);

    long left = std::min(x1, x2);
    long top = std::min(y1, y2);
    long right = std::max(x1, x2);
    long bottom = std::max(y1, y2);

    Rectangle(hdc, left, top, right, bottom);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

void EllipseShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, isSelected ? 2 : 1, isSelected ? RGB(0, 0, 255) : RGB(0, 0, 0));
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH); //Прозорий фон
    HGDIOBJ oldPen = SelectObject(hdc, hPen);
    HGDIOBJ oldBrush = SelectObject(hdc, hBrush);

    long left = std::min(x1, x2);
    long top = std::min(y1, y2);
    long right = std::max(x1, x2);
    long bottom = std::max(y1, y2);

    Ellipse(hdc, left, top, right, bottom);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

void LineWithCirclesShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, isSelected ? 2 : 1, isSelected ? RGB(0, 0, 255) : RGB(0, 0, 0));
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HGDIOBJ oldPen = SelectObject(hdc, hPen);
    HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
    
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
    
    int r = 10;
    Ellipse(hdc, x1 - r, y1 - r, x1 + r, y1 + r);
    Ellipse(hdc, x2 - r, y2 - r, x2 + r, y2 + r);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

void CubeShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, isSelected ? 2 : 1, isSelected ? RGB(0, 0, 255) : RGB(0, 0, 0));
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH); //Куб - прозорий каркас
    HGDIOBJ oldPen = SelectObject(hdc, hPen);
    HGDIOBJ oldBrush = SelectObject(hdc, hBrush);

    long x_min = std::min(x1, x2);
    long y_min = std::min(y1, y2);
    long x_max = std::max(x1, x2);
    long y_max = std::max(y1, y2);

    int w = x_max - x_min;
    int h = y_max - y_min;
    int offset = w < h ? w / 3 : h / 3;
    if (offset < 5) offset = 5;

    RECT r1 = { x_min, y_min + offset, x_max - offset, y_max };
    RECT r2 = { x_min + offset, y_min, x_max, y_max - offset };

    Rectangle(hdc, r1.left, r1.top, r1.right, r1.bottom);
    Rectangle(hdc, r2.left, r2.top, r2.right, r2.bottom);

    MoveToEx(hdc, r1.left, r1.top, NULL); LineTo(hdc, r2.left, r2.top);
    MoveToEx(hdc, r1.right, r1.top, NULL); LineTo(hdc, r2.right, r2.top);
    MoveToEx(hdc, r1.left, r1.bottom, NULL); LineTo(hdc, r2.left, r2.bottom);
    MoveToEx(hdc, r1.right, r1.bottom, NULL); LineTo(hdc, r2.right, r2.bottom);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}