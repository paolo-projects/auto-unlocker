#ifndef CHECKBOX_H
#define CHECKBOX_H

#include <Windows.h>
#include <Uxtheme.h>
#include "win32/events/events.h"
#include "win32/controls/control.h"

class CheckBox : public Control
{
public:
	CheckBox(HWND parent, int menuId, LPCSTR text, int x, int y, int width, int height);
	void set(bool checked);
	bool isChecked();

private:
};

#endif