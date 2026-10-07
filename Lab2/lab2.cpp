#define UNICODE
#define _UNICODE

#include <windows.h>
#include <cmath>

//Ідентифікатори пунктів меню
#define IDM_FILE_EXIT   1001
#define IDM_POINT       2001
#define IDM_LINE        2002
#define IDM_RECT        2003
#define IDM_ELLIPSE     2004
#define IDM_HELP_ABOUT  3001

#define MAX_SHAPES 104

class Shape {
protected:
    long x1, y1, x2, y2;
public:
    Shape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0)
        : x1(x1), y1(y1), x2(x2), y2(y2) {}
    virtual ~Shape() {}
    virtual void SetCoord(long x1_, long y1_, long x2_, long y2_) {
        x1 = x1_; y1 = y1_; x2 = x2_; y2 = y2_;
    }
    virtual void Show(HDC hdc) = 0;//Чисто віртуальна функція (поліморфізм)
};

//"Крапка"
class PointShape : public Shape {
public:
    PointShape(long x1 = 0, long y1 = 0) : Shape(x1, y1, x1, y1) {}
    void Show(HDC hdc) override {
        HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
        HGDIOBJ hOldPen = SelectObject(hdc, hPen);
        MoveToEx(hdc, x1, y1, NULL);
        LineTo(hdc, x1, y1);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }
};

//"Лінія"
class LineShape : public Shape {
public:
    LineShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0) : Shape(x1, y1, x2, y2) {}
    void Show(HDC hdc) override {
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        HGDIOBJ hOldPen = SelectObject(hdc, hPen);
        MoveToEx(hdc, x1, y1, NULL);
        LineTo(hdc, x2, y2);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }
};

//"Прямокутник" (4 mod 5 = 4: чорний контур без заповнення)
class RectangleShape : public Shape {
public:
    RectangleShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0) : Shape(x1, y1, x2, y2) {}
    void Show(HDC hdc) override {
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
        HGDIOBJ hOldPen = SelectObject(hdc, hPen);
        HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

        //Ввід по двох протилежних кутах (4 mod 2 = 0)
        Rectangle(hdc, std::min(x1, x2), std::min(y1, y2), std::max(x1, x2), std::max(y1, y2));

        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
    }
};

//"Еліпс" (4 mod 5 = 4: чорний контур, помаранчеве заповнення)
class EllipseShape : public Shape {
public:
    EllipseShape(long x1 = 0, long y1 = 0, long x2 = 0, long y2 = 0) : Shape(x1, y1, x2, y2) {}
    void Show(HDC hdc) override {
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        //Помаранчевий колір (4 mod 6 = 4 > RGB(255, 165, 0))
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 165, 0));
        HGDIOBJ hOldPen = SelectObject(hdc, hPen);
        HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);

        //Ввід від центру (x1, y1) до кута (x2, y2) (4 mod 2 = 0)
        long dx = labs(x2 - x1);
        long dy = labs(y2 - y1);
        Ellipse(hdc, x1 - dx, y1 - dy, x1 + dx, y1 + dy);

        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
        DeleteObject(hBrush);
    }
};

//Глобальні змінні стану програми
static Shape* pcshape[MAX_SHAPES]; //Статичний масив вказівників (4 mod 3 = 1)
static int g_ShapeCount = 0;
static int g_CurrentObjectType = IDM_POINT; //Поточний вибраний тип об'єкта
static BOOL g_IsDrawing = FALSE;
static POINT g_StartPoint = {0, 0};
static POINT g_CurrentPoint = {0, 0};

//Малювання "гумового" сліду під час перетягування миші
void DrawRubberTrace(HDC hdc, int type, POINT p1, POINT p2) {
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); //4 mod 4 = 0: суцільна чорна лінія
    HGDIOBJ hOldPen = SelectObject(hdc, hPen);
    HGDIOBJ hOldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
    int oldRop = SetROP2(hdc, R2_NOTXORPEN); //Інверсний режим малювання для ефекту гуми

    switch (type) {
    case IDM_POINT:
        MoveToEx(hdc, p1.x, p1.y, NULL);
        LineTo(hdc, p1.x, p1.y);
        break;
    case IDM_LINE:
        MoveToEx(hdc, p1.x, p1.y, NULL);
        LineTo(hdc, p2.x, p2.y);
        break;
    case IDM_RECT:
        Rectangle(hdc, std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::max(p1.x, p2.x), std::max(p1.y, p2.y));
        break;
    case IDM_ELLIPSE: {
        long dx = labs(p2.x - p1.x);
        long dy = labs(p2.y - p1.y);
        Ellipse(hdc, p1.x - dx, p1.y - dy, p1.x + dx, p1.y + dy);
        break;
    }
    }

    SetROP2(hdc, oldRop);
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
}

//Головна процедура вікна
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_POINT:
        case IDM_LINE:
        case IDM_RECT:
        case IDM_ELLIPSE:
            g_CurrentObjectType = LOWORD(wParam);
            break;
        case IDM_FILE_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDM_HELP_ABOUT:
            MessageBoxW(hWnd, L"Графічний редактор об'єктів\nЛабораторна робота №2\nВиконав: Вигівський Артем ІМ-51", L"Про програму", MB_OK | MB_ICONINFORMATION);
            break;
        }
        break;

    //4 mod 2 = 0: Позначка поточного типу об'єкта в меню через OnInitMenuPopup
    case WM_INITMENUPOPUP: {
        HMENU hMenu = (HMENU)wParam;
        CheckMenuItem(hMenu, IDM_POINT, MF_BYCOMMAND | (g_CurrentObjectType == IDM_POINT ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(hMenu, IDM_LINE, MF_BYCOMMAND | (g_CurrentObjectType == IDM_LINE ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(hMenu, IDM_RECT, MF_BYCOMMAND | (g_CurrentObjectType == IDM_RECT ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(hMenu, IDM_ELLIPSE, MF_BYCOMMAND | (g_CurrentObjectType == IDM_ELLIPSE ? MF_CHECKED : MF_UNCHECKED));
        break;
    }

    case WM_LBUTTONDOWN:
        g_IsDrawing = TRUE;
        g_StartPoint.x = LOWORD(lParam);
        g_StartPoint.y = HIWORD(lParam);
        g_CurrentPoint = g_StartPoint;
        SetCapture(hWnd);
        break;

    case WM_MOUSEMOVE:
        if (g_IsDrawing) {
            HDC hdc = GetDC(hWnd);
            //Стираємо попередній слід і малюємо новий
            DrawRubberTrace(hdc, g_CurrentObjectType, g_StartPoint, g_CurrentPoint);
            g_CurrentPoint.x = LOWORD(lParam);
            g_CurrentPoint.y = HIWORD(lParam);
            DrawRubberTrace(hdc, g_CurrentObjectType, g_StartPoint, g_CurrentPoint);
            ReleaseDC(hWnd, hdc);
        }
        break;

    case WM_LBUTTONUP:
        if (g_IsDrawing) {
            g_IsDrawing = FALSE;
            ReleaseCapture();

            //Стираємо останій гумовий слід
            HDC hdc = GetDC(hWnd);
            DrawRubberTrace(hdc, g_CurrentObjectType, g_StartPoint, g_CurrentPoint);
            ReleaseDC(hWnd, hdc);

            //Створення та збереження об'єкта у статичний масив
            if (g_ShapeCount < MAX_SHAPES) {
                long x2 = LOWORD(lParam);
                long y2 = HIWORD(lParam);

                switch (g_CurrentObjectType) {
                case IDM_POINT:
                    pcshape[g_ShapeCount] = new PointShape(g_StartPoint.x, g_StartPoint.y);
                    break;
                case IDM_LINE:
                    pcshape[g_ShapeCount] = new LineShape(g_StartPoint.x, g_StartPoint.y, x2, y2);
                    break;
                case IDM_RECT:
                    pcshape[g_ShapeCount] = new RectangleShape(g_StartPoint.x, g_StartPoint.y, x2, y2);
                    break;
                case IDM_ELLIPSE:
                    pcshape[g_ShapeCount] = new EllipseShape(g_StartPoint.x, g_StartPoint.y, x2, y2);
                    break;
                }
                g_ShapeCount++;
                InvalidateRect(hWnd, NULL, TRUE); //Перемальовуємо вікно
            }
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        //Малюємо всі створені об'єкти з масиву поліморфно
        for (int i = 0; i < g_ShapeCount; ++i) {
            if (pcshape[i]) {
                pcshape[i]->Show(hdc);
            }
        }
        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY:
        //Очищення пам'яті об'єктів
        for (int i = 0; i < g_ShapeCount; ++i) {
            delete pcshape[i];
            pcshape[i] = nullptr;
        }
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"Lab2GraphicEditorWindow";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassW(&wc);

    //Створення меню згідно з вимогами
    HMENU hMenuBar = CreateMenu();
    HMENU hFileMenu = CreatePopupMenu();
    HMENU hObjectsMenu = CreatePopupMenu();
    HMENU hHelpMenu = CreatePopupMenu();

    AppendMenuW(hFileMenu, MF_STRING, IDM_FILE_EXIT, L"Вихід");
    AppendMenuW(hObjectsMenu, MF_STRING, IDM_POINT, L"Крапка");
    AppendMenuW(hObjectsMenu, MF_STRING, IDM_LINE, L"Лінія");
    AppendMenuW(hObjectsMenu, MF_STRING, IDM_RECT, L"Прямокутник");
    AppendMenuW(hObjectsMenu, MF_STRING, IDM_ELLIPSE, L"Еліпс");
    AppendMenuW(hHelpMenu, MF_STRING, IDM_HELP_ABOUT, L"Про програму");

    //Меню "Об'єкти" розміщено між "Файл" та "Довідка"
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hFileMenu, L"Файл");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hObjectsMenu, L"Об'єкти");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hHelpMenu, L"Довідка");

    HWND hWnd = CreateWindowExW(
        0, CLASS_NAME, L"OOP_lab2 - Графічний редактор",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, hMenuBar, hInstance, NULL
    );

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}