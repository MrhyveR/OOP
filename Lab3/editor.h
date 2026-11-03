#pragma once

#include <windows.h>
#include <commctrl.h>
#include "shape.h"
#include "resource.h"

//Клас-контролер редактора (Інкапсуляція стану та обробки повідомлень)
class ShapeObjectsEditor {
private:
    Shape* pcshape[105]; //Статичний масив на 105 елементів
    int shapeCount;
    int currentType;
    bool isDrawing;
    POINT ptStart, ptEnd;

    void DrawRubberTrace(HWND hwnd, POINT p1, POINT p2);

public:
    ShapeObjectsEditor();
    ~ShapeObjectsEditor();

    void OnCommand(HWND hwnd, WPARAM wParam);
    LRESULT OnNotify(LPARAM lParam);
    void OnLButtonDown(HWND hwnd, LPARAM lParam);
    void OnMouseMove(HWND hwnd, LPARAM lParam);
    void OnLButtonUp(HWND hwnd, LPARAM lParam);
    void OnPaint(HWND hwnd);
};