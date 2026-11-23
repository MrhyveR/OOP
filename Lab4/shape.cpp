#define NOMINMAX
#include "shape.h"

void PointShape::Show(HDC hdc) {
    COLORREF color = isSelected ? RGB(0, 0, 255) : RGB(0, 0, 0);
    SetPixel(hdc, x2, y2, color);
}

void LineShape::Show(HDC hdc) {
    COLORREF color = isSelected ? RGB(0, 0, 255) : RGB(0, 0, 0);
    int width = isSelected ? 2 : 1;

    HPEN hPen = CreatePen(PS_SOLID, width, color);
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);

    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void RectangleShape::Show(HDC hdc) {
    COLORREF color = isSelected ? RGB(0, 0, 255) : RGB(0, 0, 0);
    int width = isSelected ? 2 : 1;

    HPEN hPen = CreatePen(PS_SOLID, width, color);
    HBRUSH hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

    long left = std::min(x1, x2);
    long top = std::min(y1, y2);
    long right = std::max(x1, x2);
    long bottom = std::max(y1, y2);

    Rectangle(hdc, left, top, right, bottom);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void EllipseShape::Show(HDC hdc) {
    COLORREF color = isSelected ? RGB(0, 0, 255) : RGB(0, 0, 0);
    int width = isSelected ? 2 : 1;

    HPEN hPen = CreatePen(PS_SOLID, width, color);
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

    long left = std::min(x1, x2);
    long top = std::min(y1, y2);
    long right = std::max(x1, x2);
    long bottom = std::max(y1, y2);

    Ellipse(hdc, left, top, right, bottom);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void LineWithCirclesShape::Show(HDC hdc) {
    long origX1 = x1, origY1 = y1, origX2 = x2, origY2 = y2;

    int r = 10;
    long dx = origX2 - origX1;
    long dy = origY2 - origY1;
    double length = std::sqrt((double)(dx * dx + dy * dy));

    if (length > r) {
        long startX = origX1 + (long)(r * dx / length);
        long startY = origY1 + (long)(r * dy / length);
        long endX = origX2 - (long)(r * dx / length);
        long endY = origY2 - (long)(r * dy / length);

        Set(startX, startY, endX, endY);
        LineShape::Show(hdc);
    }

    Set(origX1 - r, origY1 - r, origX1 + r, origY1 + r);
    EllipseShape::Show(hdc);

    Set(origX2 - r, origY2 - r, origX2 + r, origY2 + r);
    EllipseShape::Show(hdc);

    Set(origX1, origY1, origX2, origY2);
}

void CubeShape::Show(HDC hdc) {
    long origX1 = x1, origY1 = y1, origX2 = x2, origY2 = y2;

    long x_min = std::min(origX1, origX2);
    long y_min = std::min(origY1, origY2);
    long x_max = std::max(origX1, origX2);
    long y_max = std::max(origY1, origY2);

    long w = x_max - x_min;
    long h = y_max - y_min;
    long offset = std::min(w, h) / 3;
    if (offset < 5) offset = 5;

    RECT front = { x_min,          y_min + offset, x_max - offset, y_max };
    RECT back  = { x_min + offset, y_min,          x_max,          y_max - offset };

    Set(front.left,  front.top,    front.right, front.top);    LineShape::Show(hdc);
    Set(front.right, front.top,    front.right, front.bottom); LineShape::Show(hdc);
    Set(front.right, front.bottom, front.left,  front.bottom); LineShape::Show(hdc);
    Set(front.left,  front.bottom, front.left,  front.top);    LineShape::Show(hdc);

    Set(back.left,  back.top,    back.right, back.top);    LineShape::Show(hdc);
    Set(back.right, back.top,    back.right, back.bottom); LineShape::Show(hdc);
    Set(back.right, back.bottom, back.left,  back.bottom); LineShape::Show(hdc);
    Set(back.left,  back.bottom, back.left,  back.top);    LineShape::Show(hdc);

    Set(front.left,  front.top,    back.left,   back.top);    LineShape::Show(hdc);
    Set(front.right, front.top,    back.right,  back.top);    LineShape::Show(hdc);
    Set(front.left,  front.bottom, back.left,   back.bottom); LineShape::Show(hdc);
    Set(front.right, front.bottom, back.right,  back.bottom); LineShape::Show(hdc);

    Set(origX1, origY1, origX2, origY2);
}