#include "win32\controls\editbox.h"

EditBox::EditBox(HWND parent, int menuId, LPCSTR text, int x, int y, int width, int height)
{
	hWnd = CreateWindow(
		"Edit",
		text,
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		x, y, width, height,
		parent,
		(HMENU)menuId,
		(HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
		NULL
	);

	applySystemFont();
}

void EditBox::setReadOnly(bool readOnly)
{
	SendMessage(hWnd, EM_SETREADONLY, readOnly, 0);
}

bool EditBox::isReadOnly() const
{
	return GetWindowLongPtr(hWnd, GWL_STYLE) & ES_READONLY;
}

void EditBox::setText(std::string text)
{
	SetWindowText(hWnd, text.c_str());
}

std::string EditBox::getText() const
{
	char buffer[4096];
	GetWindowText(hWnd, buffer, 4096);
	return std::string(buffer);
}
