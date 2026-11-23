#pragma once
#include <windows.h>
#include <algorithm>
#include <cmath>

class Shape {
protected:
    long x1, y1, x2, y2;
    bool isSelected;
public:
    Shape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0)
        : x1(x1), y1(y1), x2(x2), y2(y2), isSelected(false) {}
    virtual ~Shape() {}

    virtual void Show(HDC hdc) = 0;
    virtual const wchar_t* GetName() const = 0;

    virtual void Set(long nx1, long ny1, long nx2, long ny2) {
        x1 = nx1; y1 = ny1; x2 = nx2; y2 = ny2;
    }

    long GetX1() const { return x1; }
    long GetY1() const { return y1; }
    long GetX2() const { return x2; }
    long GetY2() const { return y2; }

    void SetSelected(bool sel) { isSelected = sel; }
    bool IsSelected() const { return isSelected; }
};

class PointShape : public virtual Shape {
public:
    PointShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0) : Shape(x1, y1, x2, y2) {}
    void Show(HDC hdc) override;
    const wchar_t* GetName() const override { return L"PointShape"; }
};

class LineShape : public virtual Shape {
public:
    LineShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0) : Shape(x1, y1, x2, y2) {}
    void Show(HDC hdc) override;
    const wchar_t* GetName() const override { return L"LineShape"; }
};

class RectangleShape : public virtual Shape {
public:
    RectangleShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0) : Shape(x1, y1, x2, y2) {}
    void Show(HDC hdc) override;
    const wchar_t* GetName() const override { return L"RectShape"; }
};

class EllipseShape : public virtual Shape {
public:
    EllipseShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0) : Shape(x1, y1, x2, y2) {}
    void Show(HDC hdc) override;
    const wchar_t* GetName() const override { return L"EllipseShape"; }
};

class LineWithCirclesShape : public LineShape, public EllipseShape {
public:
    LineWithCirclesShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0)
        : Shape(x1, y1, x2, y2), LineShape(x1, y1, x2, y2), EllipseShape(x1, y1, x2, y2) {}
    void Show(HDC hdc) override;
    const wchar_t* GetName() const override { return L"LineOOShape"; }
};

class CubeShape : public RectangleShape, public LineShape {
public:
    CubeShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0)
        : Shape(x1, y1, x2, y2), RectangleShape(x1, y1, x2, y2), LineShape(x1, y1, x2, y2) {}
    void Show(HDC hdc) override;
    const wchar_t* GetName() const override { return L"CubeShape"; }
};
