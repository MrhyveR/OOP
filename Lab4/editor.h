#pragma once
#include "shape.h"

class MyEditor {
private:
    Shape** pcshape;
    int shapeCount;
    int capacity;
    int currentType;
    bool isDrawing;
    POINT ptStart, ptEnd;

    void EnsureCapacity();

public:
    MyEditor();
    ~MyEditor();

    void SetCurrentType(int type);
    int GetCurrentType() const;

    void OnCommand(HWND hwnd, WPARAM wParam);
    LRESULT OnNotify(LPARAM lParam);

    void OnLButtonDown(HWND hwnd, LPARAM lParam);
    void OnMouseMove(HWND hwnd, LPARAM lParam);
    void OnLButtonUp(HWND hwnd, LPARAM lParam);
    void OnPaint(HWND hwnd);

private:
    //Змінює тип параметра з HWND на HDC
    void DrawRubberTrace(HDC hdc, POINT p1, POINT p2);
};