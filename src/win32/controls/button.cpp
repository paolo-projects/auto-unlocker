#include "win32\controls\button.h"

Button::Button(HWND parent, int menuId, LPCSTR text, int x, int y, int width, int height)
{
	parentHwnd = parent;
	hWnd = CreateWindow(
		"BUTTON",
		text,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT,
		x, y, width, height,
		parent,
		(HMENU)menuId,
		(HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
		NULL
	);

	applySystemFont();
}
