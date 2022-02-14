#include "..\..\..\include\win32\controls\groupbox.h"

GroupBox::GroupBox(HWND parent, LPCSTR text, int x, int y, int width, int height)
{
	hWnd = CreateWindow(
		"Button",
		text,
		WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		x, y, width, height,
		parent,
		NULL,
		(HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
		NULL
	);

	applySystemFont();
}
