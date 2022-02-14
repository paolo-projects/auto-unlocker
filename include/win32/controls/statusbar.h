#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <Windows.h>
#include <CommCtrl.h>
#include "win32/controls/control.h"

class StatusBar : Control
{
public:
	StatusBar(HWND parent, int menuId, LPCSTR text);

	void setText(LPCSTR text);
};

#endif