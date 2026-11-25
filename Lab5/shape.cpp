#define NOMINMAX
#include "shape.h"

void PointShape::Show(HDC hdc) {
    SetPixel(hdc, x2, y2, isSelected ? RGB(0, 0, 255) : RGB(0, 0, 0));
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
    COLORREF color = isSelected ? RGB(0, 0, 255) : RGB(0, 0, 0);
    int width = isSelected ? 2 : 1;

    HPEN hPen = CreatePen(PS_SOLID, width, color);
    HBRUSH hBrush = (HBRUSH)GetStockObject(useFill ? WHITE_BRUSH : NULL_BRUSH);
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
    HPEN hPen = CreatePen(PS_SOLID, isSelected ? 2 : 1, isSelected ? RGB(0, 0, 255) : RGB(0, 0, 0));
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH); //Прозорий фон
    HGDIOBJ oldPen = SelectObject(hdc, hPen);
    HGDIOBJ oldBrush = SelectObject(hdc, hBrush);

    Ellipse(hdc, std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2));

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

void LineWithCirclesShape::Show(HDC hdc) {
    long origX1 = x1, origY1 = y1, origX2 = x2, origY2 = y2;

    int r = 10;
    long dx = origX2 - origX1;
    long dy = origY2 - origY1;
    double length = std::sqrt((double)dx * dx + (double)dy * dy);

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
    bool prevFill = useFill;
    useFill = false; //Каркас куба без заповнення

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

    //2 прямокутники через RectangleShape::Show
    Set(front.left, front.top, front.right, front.bottom);
    RectangleShape::Show(hdc);

    Set(back.left, back.top, back.right, back.bottom);
    RectangleShape::Show(hdc);

    //4 лінії між гранями через LineShape::Show
    Set(front.left,  front.top,    back.left,   back.top);    LineShape::Show(hdc);
    Set(front.right, front.top,    back.right,  back.top);    LineShape::Show(hdc);
    Set(front.left,  front.bottom, back.left,   back.bottom); LineShape::Show(hdc);
    Set(front.right, front.bottom, back.right,  back.bottom); LineShape::Show(hdc);

    useFill = prevFill;
    Set(origX1, origY1, origX2, origY2);
}