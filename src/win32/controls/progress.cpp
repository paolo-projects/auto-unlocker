#include "win32\controls\progress.h"


Progress::Progress(HWND parent, int menuId, int x, int y, int width, int height)
{
	hWnd = CreateWindow(
		PROGRESS_CLASS,
		"",
		WS_CHILD | WS_VISIBLE,
		x, y, width, height,
		parent,
		(HMENU)menuId,
		(HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
		NULL
	);

	applySystemFont();
}

void Progress::setRange(int range)
{
	SendMessage(hWnd, PBM_SETRANGE, 0, MAKELPARAM(0, range));
}

void Progress::setStep(int step)
{
	SendMessage(hWnd, PBM_SETSTEP, (WPARAM)step, 0);
}

void Progress::setProgress(int progress)
{
	SendMessage(hWnd, PBM_SETPOS, (WPARAM)progress, 0);
}

void Progress::advance()
{
	SendMessage(hWnd, PBM_STEPIT, 0, 0);
}
