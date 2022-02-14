#ifndef BUTTON_H
#define BUTTON_H

#include <Windows.h>
#include <Uxtheme.h>
#include "win32/events/events.h"
#include "win32/controls/control.h"

class Button : public Control
{
public:
	Button(HWND parent, int menuId, LPCSTR text, int x, int y, int width, int height);

private:
};

#endif