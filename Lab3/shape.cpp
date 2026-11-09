#define NOMINMAX
#include "shape.h"

//Shape
Shape::Shape(long x1, long y1, long x2, long y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}
Shape::~Shape() {}

//PointShape
PointShape::PointShape(long x1, long y1, long x2, long y2) : Shape(x1, y1, x2, y2) {}
void PointShape::Show(HDC hdc) {
    SetPixel(hdc, x2, y2, RGB(0, 0, 0));
}

// LineShape
LineShape::LineShape(long x1, long y1, long x2, long y2) : Shape(x1, y1, x2, y2) {}
void LineShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

//RectangleShape (Ж = 5: ввід від центру до кута, біле заповнення)
RectangleShape::RectangleShape(long x1, long y1, long x2, long y2) : Shape(x1, y1, x2, y2) {}
void RectangleShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);//Біле заповнення (5 mod 5 = 0)
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

    long left = x1 - std::abs(x2 - x1);
    long right = x1 + std::abs(x2 - x1);
    long top = y1 - std::abs(y2 - y1);
    long bottom = y1 + std::abs(y2 - y1);

    Rectangle(hdc, left, top, right, bottom);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
}

//EllipseShape (Ж = 5: ввід по двох протилежних кутах, без заповнення)
EllipseShape::EllipseShape(long x1, long y1, long x2, long y2) : Shape(x1, y1, x2, y2) {}
void EllipseShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);//Без заповнення (5 mod 5 = 0)
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

    Ellipse(hdc, std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2));

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
}