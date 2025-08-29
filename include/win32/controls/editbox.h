#ifndef EDITBOX_H
#define EDITBOX_H

#include <Windows.h>
#include <Uxtheme.h>
#include <string>
#include "win32/controls/control.h"

class EditBox : public Control 
{
public:
	EditBox(HWND parent, int menuId, LPCSTR text, int x, int y, int width, int height);

	void setReadOnly(bool readOnly);
	bool isReadOnly() const;
	void setText(std::string text);
	std::string getText() const;
private:
};

#endif