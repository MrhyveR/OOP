#pragma once

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <cmath>
#include <algorithm>

//Базовий абстрактний клас фігури
class Shape {
protected:
    long x1, y1, x2, y2;
public:
    Shape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0);
    virtual ~Shape();
    virtual void SetCoord(long x1_, long y1_, long x2_, long y2_);
    virtual void Show(HDC hdc) = 0; //поліморфізм
};

//"Крапка"
class PointShape : public Shape {
public:
    PointShape(long x1 = 0, long y1 = 0);
    void Show(HDC hdc) override;
};

//"Лінія"
class LineShape : public Shape {
public:
    LineShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0);
    void Show(HDC hdc) override;
};

//"Прямокутник" (4 mod 5 = 4: чорний контур без заповнення)
class RectangleShape : public Shape {
public:
    RectangleShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0);
    void Show(HDC hdc) override;
};

//"Еліпс" (4 mod 5 = 4: чорний контур, помаранчеве заповнення)
class EllipseShape : public Shape {
public:
    EllipseShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0);
    void Show(HDC hdc) override;
};