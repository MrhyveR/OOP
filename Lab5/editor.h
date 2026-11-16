#pragma once
#include "shape.h"
#include <fstream>

class MyEditor {
private:
    static MyEditor* instance;

    Shape** pcshape;
    int shapeCount;
    int capacity;
    int currentType;
    bool isDrawing;
    POINT ptStart, ptEnd;

    MyEditor();
    ~MyEditor();

    MyEditor(const MyEditor&) = delete;
    MyEditor& operator=(const MyEditor&) = delete;

    void EnsureCapacity();

public:
    static MyEditor* GetInstance();
    static void DestroyInstance();

    void SetCurrentType(int type);
    int GetCurrentType() const;
    const char* GetCurrentTypeName() const;

    void OnCommand(HWND hwnd, WPARAM wParam);
    LRESULT OnNotify(LPARAM lParam);

    void OnLButtonDown(HWND hwnd, LPARAM lParam);
    void OnMouseMove(HWND hwnd, LPARAM lParam);
    void OnLButtonUp(HWND hwnd, LPARAM lParam);
    void OnPaint(HWND hwnd);

    int GetShapeCount() const { return shapeCount; }
    Shape* const* GetShapes() const { return pcshape; }
    
    void RemoveShape(int index);
    void SelectShape(int index);
    void ClearAll();

    void SaveToFile(const char* filename);
    void LoadFromFile(const char* filename);

private:
    void DrawRubberTrace(HDC hdc, POINT p1, POINT p2);
};