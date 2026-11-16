#pragma once
#include <windows.h>
#include <commctrl.h>

class MyTable {
private:
    HWND hwndDlg;
    HWND hwndParent;
    static LRESULT CALLBACK StaticDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    MyTable();
    ~MyTable();

    void Create(HINSTANCE hInstance, HWND hParent);
    void Show();
    void Hide();
    void Toggle(HINSTANCE hInstance, HWND hParent);
    void UpdateTable();
    bool IsVisible() const;
    HWND GetHwnd() const { return hwndDlg; }
};