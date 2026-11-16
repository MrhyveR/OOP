#include "table.h"
#include "editor.h"
#include <commctrl.h>
#include <cstdio>

#define ID_LISTVIEW 1001
#define ID_BTN_DELETE 1002
#define ID_BTN_CLOSE 1003

MyTable::MyTable() : hwndDlg(nullptr), hwndParent(nullptr) {}

MyTable::~MyTable() {
    if (hwndDlg) DestroyWindow(hwndDlg);
}

void MyTable::Create(HINSTANCE hInstance, HWND hParent) {
    hwndParent = hParent;
    if (hwndDlg && IsWindow(hwndDlg)) return;

    hwndDlg = CreateWindowExA(
        WS_EX_TOOLWINDOW,
        "#32770",
        "Object table",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 520, 360,
        hParent, NULL, hInstance, this
    );

    SetWindowLongPtrA(hwndDlg, GWLP_USERDATA, (LONG_PTR)this);
    SetWindowLongPtrA(hwndDlg, GWLP_WNDPROC, (LONG_PTR)StaticDlgProc);

    RECT rc;
    GetClientRect(hwndDlg, &rc);

    HWND hList = CreateWindowExA(
        0, WC_LISTVIEWA, "",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
        15, 15, rc.right - 30, rc.bottom - 75,
        hwndDlg, (HMENU)ID_LISTVIEW, hInstance, NULL
    );

    CreateWindowExA(
        0, "BUTTON", "Delete",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        rc.right - 190, rc.bottom - 48, 80, 30,
        hwndDlg, (HMENU)ID_BTN_DELETE, hInstance, NULL
    );

    CreateWindowExA(
        0, "BUTTON", "Close",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        rc.right - 95, rc.bottom - 48, 80, 30,
        hwndDlg, (HMENU)ID_BTN_CLOSE, hInstance, NULL
    );

    LVCOLUMNA lvc = {};
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    const char* headers[] = { "Type", "x", "y", "x", "y" };
    int widths[] = { 130, 70, 70, 70, 70 };

    for (int i = 0; i < 5; i++) {
        lvc.iSubItem = i;
        lvc.pszText = (LPSTR)headers[i];
        lvc.cx = widths[i];
        SendMessageA(hList, LVM_INSERTCOLUMNA, i, (LPARAM)&lvc);
    }

    UpdateTable();
}

LRESULT CALLBACK MyTable::StaticDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    MyTable* pThis = (MyTable*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);
    if (pThis) return pThis->DlgProc(hwnd, uMsg, wParam, lParam);
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MyTable::DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_SIZE: {
            HWND hList = GetDlgItem(hwnd, ID_LISTVIEW);
            HWND hBtnDel = GetDlgItem(hwnd, ID_BTN_DELETE);
            HWND hBtnClose = GetDlgItem(hwnd, ID_BTN_CLOSE);
            if (hList && hBtnDel && hBtnClose) {
                RECT rc;
                GetClientRect(hwnd, &rc);
                MoveWindow(hList, 15, 15, rc.right - 30, rc.bottom - 75, TRUE);
                MoveWindow(hBtnDel, rc.right - 190, rc.bottom - 48, 80, 30, TRUE);
                MoveWindow(hBtnClose, rc.right - 95, rc.bottom - 48, 80, 30, TRUE);
            }
            break;
        }
        case WM_NOTIFY: {
            LPNMHDR pnmh = (LPNMHDR)lParam;
            if (pnmh->idFrom == ID_LISTVIEW && pnmh->code == LVN_ITEMCHANGED) {
                LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
                if ((pnmv->uChanged & LVIF_STATE) && (pnmv->uNewState & LVIS_SELECTED)) {
                    MyEditor::GetInstance()->SelectShape(pnmv->iItem);
                    InvalidateRect(hwndParent, NULL, TRUE);
                }
            }
            break;
        }
        case WM_COMMAND: {
            int id = LOWORD(wParam);
            if (id == ID_BTN_CLOSE) {
                Hide();
            } else if (id == ID_BTN_DELETE) {
                HWND hList = GetDlgItem(hwnd, ID_LISTVIEW);
                int selIndex = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
                if (selIndex != -1) {
                    MyEditor::GetInstance()->RemoveShape(selIndex);
                    UpdateTable();
                    InvalidateRect(hwndParent, NULL, TRUE);
                }
            }
            break;
        }
        case WM_CLOSE:
            Hide();
            return 0;
    }
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

void MyTable::Show() { if (hwndDlg) ShowWindow(hwndDlg, SW_SHOW); }
void MyTable::Hide() { if (hwndDlg) ShowWindow(hwndDlg, SW_HIDE); }
bool MyTable::IsVisible() const { return hwndDlg && IsWindowVisible(hwndDlg); }

void MyTable::Toggle(HINSTANCE hInstance, HWND hParent) {
    if (!hwndDlg || !IsWindow(hwndDlg)) Create(hInstance, hParent);
    else {
        if (IsWindowVisible(hwndDlg)) Hide();
        else Show();
    }
}

void MyTable::UpdateTable() {
    if (!hwndDlg) return;
    HWND hList = GetDlgItem(hwndDlg, ID_LISTVIEW);
    if (!hList) return;

    ListView_DeleteAllItems(hList);

    MyEditor* editor = MyEditor::GetInstance();
    int count = editor->GetShapeCount();
    Shape* const* shapes = editor->GetShapes();

    for (int i = 0; i < count; i++) {
        if (!shapes[i]) continue;

        LVITEMA lvi = {};
        lvi.mask = LVIF_TEXT;
        lvi.iItem = i;
        lvi.iSubItem = 0;
        
        //Перетворюю назву фігури на ANSI
        char typeNameA[64];
        WideCharToMultiByte(CP_ACP, 0, shapes[i]->GetName(), -1, typeNameA, sizeof(typeNameA), NULL, NULL);
        lvi.pszText = typeNameA;
        
        SendMessageA(hList, LVM_INSERTITEMA, 0, (LPARAM)&lvi);

        char buf[32];
        LVITEMA lviSub = {};
        lviSub.iItem = i;
        lviSub.mask = LVIF_TEXT;

        sprintf(buf, "%ld", shapes[i]->GetX1());
        lviSub.iSubItem = 1; lviSub.pszText = buf;
        SendMessageA(hList, LVM_SETITEMTEXTA, i, (LPARAM)&lviSub);

        sprintf(buf, "%ld", shapes[i]->GetY1());
        lviSub.iSubItem = 2; lviSub.pszText = buf;
        SendMessageA(hList, LVM_SETITEMTEXTA, i, (LPARAM)&lviSub);

        sprintf(buf, "%ld", shapes[i]->GetX2());
        lviSub.iSubItem = 3; lviSub.pszText = buf;
        SendMessageA(hList, LVM_SETITEMTEXTA, i, (LPARAM)&lviSub);

        sprintf(buf, "%ld", shapes[i]->GetY2());
        lviSub.iSubItem = 4; lviSub.pszText = buf;
        SendMessageA(hList, LVM_SETITEMTEXTA, i, (LPARAM)&lviSub);
    }
}