#include "..\..\..\include\win32\controls\label.h"

Label::Label(HWND parent, int menuId, LPCSTR text, int x, int y, int width, int height)
{
	parentHwnd = parent;
	hWnd = CreateWindow(
		"STATIC",
		text,
		WS_VISIBLE | WS_CHILD,
		x, y, width, height,
		parent,
		(HMENU)menuId,
		(HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
		NULL
	);

	applySystemFont();
}
