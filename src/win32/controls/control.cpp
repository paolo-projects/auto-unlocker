#include "win32\controls\control.h"

Control::Control()
{
}

void Control::addEventListener(EventType eventType, EventCallback callback)
{
	events[eventType] = callback;
}

void Control::triggerEvent(EventType eventType)
{
	std::map<EventType, EventCallback>::iterator e;
	if ((e = events.find(eventType)) != events.end()) {
		e->second();
	}
}

void Control::setEnabled(bool enabled)
{
	//SendMessage(hWnd, WM_ENABLE, enabled ? TRUE : FALSE, 0);
	EnableWindow(hWnd, enabled);
}

bool Control::isEnabled()
{
	return IsWindowEnabled(hWnd);
}

HWND Control::getHwnd() const
{
	return hWnd;
}

void Control::applySystemFont()
{
	// Set system font
	NONCLIENTMETRICS metrics;
	metrics.cbSize = sizeof(metrics);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0);
	HFONT sysFont = CreateFontIndirect(&metrics.lfMessageFont);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)sysFont, MAKELPARAM(TRUE, 0));
}
