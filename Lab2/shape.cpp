#include "shape.h"

Shape::Shape(long x1, long y1, long x2, long y2)
    : x1(x1), y1(y1), x2(x2), y2(y2) {}

Shape::~Shape() {}

void Shape::SetCoord(long x1_, long y1_, long x2_, long y2_) {
    x1 = x1_; y1 = y1_; x2 = x2_; y2 = y2_;
}

PointShape::PointShape(long x1, long y1) : Shape(x1, y1, x1, y1) {}

void PointShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x1, y1);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

LineShape::LineShape(long x1, long y1, long x2, long y2) : Shape(x1, y1, x2, y2) {}

void LineShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

RectangleShape::RectangleShape(long x1, long y1, long x2, long y2) : Shape(x1, y1, x2, y2) {}

void RectangleShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

    Rectangle(hdc, std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2));

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
}

EllipseShape::EllipseShape(long x1, long y1, long x2, long y2) : Shape(x1, y1, x2, y2) {}

void EllipseShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 165, 0));
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

    long dx = labs(x2 - x1);
    long dy = labs(y2 - y1);
    Ellipse(hdc, x1 - dx, y1 - dy, x1 + dx, y1 + dy);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}