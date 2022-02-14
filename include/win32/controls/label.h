#ifndef LABEL_H
#define LABEL_H

#include <Windows.h>
#include "win32\controls\control.h"

class Label : public Control
{
public:
	Label(HWND parent, int menuId, LPCSTR text, int x, int y, int width, int height);

private:
};

#endif