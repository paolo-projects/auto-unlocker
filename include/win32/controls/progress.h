#ifndef PROGRESS_H
#define PROGRESS_H

#include <Windows.h>
#include <CommCtrl.h>
#include "win32/controls/control.h"

class Progress : public Control 
{
public:
	Progress(HWND parent, int menuId, int x, int y, int width, int height);
	void setRange(int range);
	void setStep(int step);
	void setProgress(int progress);
	void advance();

private:
};

#endif