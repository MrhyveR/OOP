#pragma once

#include <windows.h>
#include <algorithm>
#include <cmath>

//Базовий абстрактний клас фігури
class Shape {
protected:
    long x1, y1, x2, y2;
public:
    Shape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0);
    virtual ~Shape();
    virtual void Show(HDC hdc) = 0;
};

//Крапка
class PointShape : public Shape {
public:
    PointShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0);
    void Show(HDC hdc) override;
};

//Лінія
class LineShape : public Shape {
public:
    LineShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0);
    void Show(HDC hdc) override;
};

//Прямокутник
class RectangleShape : public Shape {
public:
    RectangleShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0);
    void Show(HDC hdc) override;
};

//Еліпс
class EllipseShape : public Shape {
public:
    EllipseShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0);
    void Show(HDC hdc) override;
};