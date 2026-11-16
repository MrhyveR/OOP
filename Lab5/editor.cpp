#define NOMINMAX
#include <windows.h>
#include <commctrl.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include "editor.h"
#include "resource.h"

MyEditor* MyEditor::instance = nullptr;

MyEditor* MyEditor::GetInstance() {
    if (!instance) instance = new MyEditor();
    return instance;
}

void MyEditor::DestroyInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

MyEditor::MyEditor() : pcshape(nullptr), shapeCount(0), capacity(0), currentType(IDM_POINT), isDrawing(false) {
    ptStart.x = ptStart.y = 0;
    ptEnd.x = ptEnd.y = 0;
}

MyEditor::~MyEditor() {
    ClearAll();
}

void MyEditor::ClearAll() {
    for (int i = 0; i < shapeCount; i++) delete pcshape[i];
    delete[] pcshape;
    pcshape = nullptr;
    shapeCount = 0;
    capacity = 0;
}

void MyEditor::EnsureCapacity() {
    if (capacity == 0) {
        capacity = 8;
        pcshape = new Shape*[capacity];
    } else if (shapeCount >= capacity) {
        capacity *= 2;
        Shape** newArr = new Shape*[capacity];
        for (int i = 0; i < shapeCount; i++) {
            newArr[i] = pcshape[i];
        }
        delete[] pcshape;
        pcshape = newArr;
    }
}

void MyEditor::SetCurrentType(int type) { currentType = type; }
int MyEditor::GetCurrentType() const { return currentType; }

const char* MyEditor::GetCurrentTypeName() const {
    switch (currentType) {
        case IDM_POINT:      return "Point";
        case IDM_LINE:       return "Line";
        case IDM_RECT:       return "Rectangle";
        case IDM_ELLIPSE:    return "Ellipse";
        case IDM_LINECIRCLE: return "Line with circles";
        case IDM_CUBE:       return "Cube";
        default:             return "Unknown";
    }
}

void MyEditor::OnCommand(HWND hwnd, WPARAM wParam) {
    int id = LOWORD(wParam);
    if (id >= IDM_POINT && id <= IDM_CUBE) {
        SetCurrentType(id);
    } else if (id == IDM_ABOUT) {
        MessageBoxA(hwnd, "Graphic Editor (Lab 5)\nVariant 4: Singleton + Table", "About", MB_OK | MB_ICONINFORMATION);
    } else if (id == IDM_SAVE) {
        char szFile[260] = { 0 };
        OPENFILENAMEA ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
        ofn.lpstrDefExt = "txt";
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
        if (GetSaveFileNameA(&ofn)) {
            SaveToFile(szFile);
        }
    } else if (id == IDM_OPEN) {
        char szFile[260] = { 0 };
        OPENFILENAMEA ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        if (GetOpenFileNameA(&ofn)) {
            LoadFromFile(szFile);
            InvalidateRect(hwnd, NULL, TRUE);
        }
    }
}

LRESULT MyEditor::OnNotify(LPARAM lParam) {
    LPNMHDR pnmh = (LPNMHDR)lParam;
    if (pnmh && pnmh->code == TTN_GETDISPINFOA) {
        LPTOOLTIPTEXTA lpttt = (LPTOOLTIPTEXTA)lParam;
        const char* text = "";
        switch (lpttt->hdr.idFrom) {
            case IDM_POINT:        text = "Point"; break;
            case IDM_LINE:         text = "Line"; break;
            case IDM_RECT:         text = "Rectangle"; break;
            case IDM_ELLIPSE:      text = "Ellipse"; break;
            case IDM_LINECIRCLE:   text = "Line with circles"; break;
            case IDM_CUBE:         text = "Cube"; break;
        }
        lstrcpynA(lpttt->szText, text, sizeof(lpttt->szText));
    }
    return 0;
}

void MyEditor::OnLButtonDown(HWND hwnd, LPARAM lParam) {
    isDrawing = true;
    ptStart.x = LOWORD(lParam);
    ptStart.y = HIWORD(lParam);
    ptEnd = ptStart;
    SetCapture(hwnd);
}

void MyEditor::OnMouseMove(HWND hwnd, LPARAM lParam) {
    if (!isDrawing) return;

    ptEnd.x = LOWORD(lParam);
    ptEnd.y = HIWORD(lParam);

    InvalidateRect(hwnd, NULL, FALSE);
}

void MyEditor::OnLButtonUp(HWND hwnd, LPARAM lParam) {
    if (!isDrawing) return;
    isDrawing = false;
    ReleaseCapture();

    ptEnd.x = LOWORD(lParam);
    ptEnd.y = HIWORD(lParam);

    Shape* newShape = nullptr;
    switch (currentType) {
        case IDM_POINT:
            newShape = new PointShape(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
            break;
        case IDM_LINE:
            newShape = new LineShape(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
            break;
        case IDM_RECT:
        newShape = new RectangleShape(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
            break;
        case IDM_ELLIPSE:
        newShape = new EllipseShape(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
            break;
        case IDM_LINECIRCLE:
            newShape = new LineWithCirclesShape(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
            break;
        case IDM_CUBE:
            newShape = new CubeShape(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
            break;
    }

    if (newShape) {
        EnsureCapacity();
        pcshape[shapeCount++] = newShape;
    }

    InvalidateRect(hwnd, NULL, TRUE);
}

void MyEditor::OnPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT rc;
    GetClientRect(hwnd, &rc);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    HBRUSH hWhiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
    FillRect(hdcMem, &rc, hWhiteBrush);

    for (int i = 0; i < shapeCount; i++) {
        if (pcshape[i]) pcshape[i]->Show(hdcMem);
    }

    if (isDrawing) {
        DrawRubberTrace(hdcMem, ptStart, ptEnd);
    }

    BitBlt(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);

    EndPaint(hwnd, &ps);
}

void MyEditor::DrawRubberTrace(HDC hdc, POINT p1, POINT p2) {
    HPEN hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 255));
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

    switch (currentType) {
        case IDM_POINT: {
            MoveToEx(hdc, p2.x - 5, p2.y, NULL);
            LineTo(hdc, p2.x + 5, p2.y);
            MoveToEx(hdc, p2.x, p2.y - 5, NULL);
            LineTo(hdc, p2.x, p2.y + 5);
            break;
        }
        case IDM_LINE: {
            MoveToEx(hdc, p1.x, p1.y, NULL);
            LineTo(hdc, p2.x, p2.y);
            break;
        }
        case IDM_RECT: {
            long left = std::min(p1.x, p2.x);
            long top = std::min(p1.y, p2.y);
            long right = std::max(p1.x, p2.x);
            long bottom = std::max(p1.y, p2.y);
            Rectangle(hdc, left, top, right, bottom);
            break;
        }
        case IDM_ELLIPSE: {
            long left = std::min(p1.x, p2.x);
            long top = std::min(p1.y, p2.y);
            long right = std::max(p1.x, p2.x);
            long bottom = std::max(p1.y, p2.y);
            Ellipse(hdc, left, top, right, bottom);
            break;
        }
        case IDM_LINECIRCLE: {
            int r = 10;
            long dx = p2.x - p1.x;
            long dy = p2.y - p1.y;
            double length = std::sqrt((double)(dx * dx + dy * dy));

            if (length > r) {
                long startX = p1.x + (long)(r * dx / length);
                long startY = p1.y + (long)(r * dy / length);
                long endX = p2.x - (long)(r * dx / length);
                long endY = p2.y - (long)(r * dy / length);

                MoveToEx(hdc, startX, startY, NULL);
                LineTo(hdc, endX, endY);
            }

            Ellipse(hdc, p1.x - r, p1.y - r, p1.x + r, p1.y + r);
            Ellipse(hdc, p2.x - r, p2.y - r, p2.x + r, p2.y + r);
            break;
        }
        case IDM_CUBE: {
            long x_min = std::min(p1.x, p2.x);
            long y_min = std::min(p1.y, p2.y);
            long x_max = std::max(p1.x, p2.x);
            long y_max = std::max(p1.y, p2.y);

            int w = x_max - x_min;
            int h = y_max - y_min;
            int offset = w < h ? w / 3 : h / 3;
            if (offset < 5) offset = 5;

            RECT r1 = { x_min, y_min + offset, x_max - offset, y_max };
            RECT r2 = { x_min + offset, y_min, x_max, y_max - offset };

            Rectangle(hdc, r1.left, r1.top, r1.right, r1.bottom);
            Rectangle(hdc, r2.left, r2.top, r2.right, r2.bottom);

            MoveToEx(hdc, r1.left, r1.top, NULL); LineTo(hdc, r2.left, r2.top);
            MoveToEx(hdc, r1.right, r1.top, NULL); LineTo(hdc, r2.right, r2.top);
            MoveToEx(hdc, r1.left, r1.bottom, NULL); LineTo(hdc, r2.left, r2.bottom);
            MoveToEx(hdc, r1.right, r1.bottom, NULL); LineTo(hdc, r2.right, r2.bottom);
            break;
        }
    }

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
}

void MyEditor::RemoveShape(int index) {
    if (index < 0 || index >= shapeCount) return;
    delete pcshape[index];
    for (int i = index; i < shapeCount - 1; i++) {
        pcshape[i] = pcshape[i + 1];
    }
    shapeCount--;
}

void MyEditor::SelectShape(int index) {
    for (int i = 0; i < shapeCount; i++) {
        if (pcshape[i]) pcshape[i]->SetSelected(i == index);
    }
}

void MyEditor::SaveToFile(const char* filename) {
    std::ofstream out(filename);
    if (!out.is_open()) return;

    for (int i = 0; i < shapeCount; i++) {
        if (!pcshape[i]) continue;
        std::string typeName = "Point";
        if (dynamic_cast<CubeShape*>(pcshape[i])) typeName = "Cube";
        else if (dynamic_cast<LineWithCirclesShape*>(pcshape[i])) typeName = "LineCircle";
        else if (dynamic_cast<RectangleShape*>(pcshape[i])) typeName = "Rect";
        else if (dynamic_cast<EllipseShape*>(pcshape[i])) typeName = "Ellipse";
        else if (dynamic_cast<LineShape*>(pcshape[i])) typeName = "Line";

        out << typeName << " " 
            << pcshape[i]->GetX1() << " " 
            << pcshape[i]->GetY1() << " " 
            << pcshape[i]->GetX2() << " " 
            << pcshape[i]->GetY2() << "\n";
    }
    out.close();
}

void MyEditor::LoadFromFile(const char* filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        MessageBoxA(NULL, "Could not open file for reading.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    //Тимчасовий массив для перевірки перед заміною
    int tempCount = 0;
    int tempCapacity = 8;
    Shape** tempShapes = new Shape*[tempCapacity];

    std::string line;
    int lineNum = 0;
    bool hasError = false;
    char errorMsg[256];

    while (std::getline(in, line)) {
        lineNum++;
        //Пропускаю порожні рядки
        if (line.find_first_not_of(" \t\r\n") == std::string::npos) continue;

        std::stringstream ss(line);
        std::string type;
        long x1, y1, x2, y2;

        //Перевіряю, чи є назва і рівно 4 координати
        if (!(ss >> type >> x1 >> y1 >> x2 >> y2)) {
            sprintf(errorMsg, "Format error in file on line %d: missing coordinates or shape name.", lineNum);
            hasError = true;
            break;
        }

        //Перевіряю підтримку назви фігури
        Shape* s = nullptr;
        if (type == "Point") s = new PointShape(x1, y1, x2, y2);
        else if (type == "Line") s = new LineShape(x1, y1, x2, y2);
        else if (type == "Rect") s = new RectangleShape(x1, y1, x2, y2);
        else if (type == "Ellipse") s = new EllipseShape(x1, y1, x2, y2);
        else if (type == "LineCircle") s = new LineWithCirclesShape(x1, y1, x2, y2);
        else if (type == "Cube") s = new CubeShape(x1, y1, x2, y2);
        else {
            sprintf(errorMsg, "Unknown shape name '%s' on line %d.", type.c_str(), lineNum);
            hasError = true;
            break;
        }

        //Додаю до тимчасового масиву
        if (tempCount >= tempCapacity) {
            tempCapacity *= 2;
            Shape** newArr = new Shape*[tempCapacity];
            for (int i = 0; i < tempCount; i++) newArr[i] = tempShapes[i];
            delete[] tempShapes;
            tempShapes = newArr;
        }
        tempShapes[tempCount++] = s;
    }
    in.close();

    if (hasError) {
        //Очищаю те, що встигли створити у тимчасовому масиві
        for (int i = 0; i < tempCount; i++) delete tempShapes[i];
        delete[] tempShapes;

        //Вивожу помилку з номером рядка, попередній малюнок залишається незмінним
        MessageBoxA(NULL, errorMsg, "File Load Error", MB_OK | MB_ICONERROR);
    } else {
        //Якщо все успішно - замінюємо поточний малюнок новими фігурами
        ClearAll();
        pcshape = tempShapes;
        shapeCount = tempCount;
        capacity = tempCapacity;
    }
}