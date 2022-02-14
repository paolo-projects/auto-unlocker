#ifndef GROUPBOX_H
#define GROUPBOX_H

#include <Windows.h>
#include <Uxtheme.h>
#include "win32/controls/control.h"

class GroupBox : public Control
{
public:
	GroupBox(HWND parent, LPCSTR text, int x, int y, int width, int height);

private:
};

#endif