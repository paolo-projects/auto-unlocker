#include "win32\controls\statusbar.h"

StatusBar::StatusBar(HWND parent, int menuId, LPCSTR text)
{
    hWnd = CreateWindowEx(
        0,                       // no extended styles
        STATUSCLASSNAME,         // name of status bar class
        (PCTSTR)NULL,           // no text when first created
        WS_CHILD | WS_VISIBLE,   // creates a visible child window
        0, 0, 0, 0,              // ignores size and position
        parent,                 // handle to parent window
        (HMENU)menuId,          // child window identifier
        (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),                   // handle to application instance
        NULL);
}

void StatusBar::setText(LPCSTR text)
{
    SetWindowText(hWnd, text);
}
