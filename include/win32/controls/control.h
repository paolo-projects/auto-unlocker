#ifndef CONTROL_H
#define CONTROL_H

#include <map>
#include <Windows.h>
#include "win32/events/events.h"

class Control
{
public:
	Control();

	void addEventListener(EventType eventType, EventCallback callback);
	void triggerEvent(EventType eventType);

	void setEnabled(bool enabled);
	bool isEnabled();

	HWND getHwnd() const;

protected:
	std::map<EventType, EventCallback> events;
	HWND hWnd = NULL;
	HWND parentHwnd = NULL;

	void applySystemFont();
};

#endif