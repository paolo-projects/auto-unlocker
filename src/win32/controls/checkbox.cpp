#include "..\..\..\include\win32\controls\checkbox.h"

CheckBox::CheckBox(HWND parent, int menuId, LPCSTR text, int x, int y, int width, int height)
{
	hWnd = CreateWindow(
		"BUTTON",
		text,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
		x, y, width, height,
		parent,
		(HMENU)menuId,
		(HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
		NULL
	);

	applySystemFont();
}

void CheckBox::set(bool checked)
{
	SendMessage(hWnd, BM_SETCHECK, checked ? BST_CHECKED : BST_UNCHECKED, 0);
}

bool CheckBox::isChecked()
{
	HRESULT res = SendMessageA(hWnd, BM_GETCHECK, 0, 0);
	return res == BST_CHECKED;
}
