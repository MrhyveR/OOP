#define NOMINMAX
#include "shape.h"

Shape::Shape(long x1, long y1, long x2, long y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}
Shape::~Shape() {}

PointShape::PointShape(long x1, long y1, long x2, long y2) : Shape(x1, y1, x2, y2) {}
void PointShape::Show(HDC hdc) {
    SetPixel(hdc, x2, y2, RGB(0, 0, 0));
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
    HBRUSH hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH); // Біле заповнення
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

EllipseShape::EllipseShape(long x1, long y1, long x2, long y2) : Shape(x1, y1, x2, y2) {}
void EllipseShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH); //Без заповнення
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

    Ellipse(hdc, std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2));

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
}

//LineWithCirclesShape (лінія від межі до межі кружечків)
LineWithCirclesShape::LineWithCirclesShape(long x1, long y1, long x2, long y2) : Shape(x1, y1, x2, y2), LineShape(x1, y1, x2, y2), EllipseShape(x1, y1, x2, y2) {}

void LineWithCirclesShape::Show(HDC hdc) {
    int r = 10; //Радіус кружечків
    long dx = x2 - x1;
    long dy = y2 - y1;
    double length = std::sqrt((double)(dx * dx + dy * dy));

    if (length > r) {
        //Зменшує довжину ліній, щоб вони починалися/закінчувалися на краях кіл
        long startX = x1 + (long)(r * dx / length);
        long startY = y1 + (long)(r * dy / length);
        long endX = x2 - (long)(r * dx / length);
        long endY = y2 - (long)(r * dy / length);

        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        HGDIOBJ hOldPen = SelectObject(hdc, hPen);
        MoveToEx(hdc, startX, startY, NULL);
        LineTo(hdc, endX, endY);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }

    //Малює кружечки на кінцях
    EllipseShape startCircle(x1 - r, y1 - r, x1 + r, y1 + r);
    startCircle.Show(hdc);

    EllipseShape endCircle(x2 - r, y2 - r, x2 + r, y2 + r);
    endCircle.Show(hdc);
}

//CubeShape (каркас куба без заповнення прямокутників)
CubeShape::CubeShape(long x1, long y1, long x2, long y2) : Shape(x1, y1, x2, y2), RectangleShape(x1, y1, x2, y2), LineShape(x1, y1, x2, y2) {}

void CubeShape::Show(HDC hdc) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH); //Без заповнення
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

    //Напівширина та напіввисота прямокутника
    long hw = std::abs(x2 - x1);
    long hh = std::abs(y2 - y1);

    //Зсув для задньої грані (вправо і вгору)
    long offsetX = hw / 2;
    long offsetY = -hh / 2;

    //Точні кути передньої грані
    long f_left = x1 - hw;
    long f_right = x1 + hw;
    long f_top = y1 - hh;
    long f_bottom = y1 + hh;

    //Малює передній прямокутник
    Rectangle(hdc, f_left, f_top, f_right, f_bottom);

    //Точні кути задньої грані
    long b_left = f_left + offsetX;
    long b_right = f_right + offsetX;
    long b_top = f_top + offsetY;
    long b_bottom = f_bottom + offsetY;

    //Малює задній прямокутник
    Rectangle(hdc, b_left, b_top, b_right, b_bottom);

    //З'єднувальні ребра між відповідними кутами граней
    MoveToEx(hdc, f_left, f_top, NULL);
    LineTo(hdc, b_left, b_top);

    MoveToEx(hdc, f_right, f_top, NULL);
    LineTo(hdc, b_right, b_top);

    MoveToEx(hdc, f_left, f_bottom, NULL);
    LineTo(hdc, b_left, b_bottom);

    MoveToEx(hdc, f_right, f_bottom, NULL);
    LineTo(hdc, b_right, b_bottom);

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
}