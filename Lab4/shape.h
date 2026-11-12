#pragma once

#include <windows.h>
#include <algorithm>
#include <cmath>

class Shape {
protected:
    long x1, y1, x2, y2;
public:
    Shape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0);
    virtual ~Shape();
    virtual void Show(HDC hdc) = 0;
};

class PointShape : public virtual Shape {
public:
    PointShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0);
    void Show(HDC hdc) override;
};

class LineShape : public virtual Shape {
public:
    LineShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0);
    void Show(HDC hdc) override;
};

class RectangleShape : public virtual Shape {
public:
    RectangleShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0);
    void Show(HDC hdc) override;
};

class EllipseShape : public virtual Shape {
public:
    EllipseShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0);
    void Show(HDC hdc) override;
};

//Нова фігура 1 лінія з кружечками
class LineWithCirclesShape : public LineShape, public EllipseShape {
public:
    LineWithCirclesShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0);
    void Show(HDC hdc) override;
};

//Нова фігура 2 каркас куба
class CubeShape : public RectangleShape, public LineShape {
public:
    CubeShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0);
    void Show(HDC hdc) override;
};