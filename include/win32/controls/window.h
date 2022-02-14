#ifndef WINDOW_H
#define WINDOW_H

#include <Windows.h>
#include <string>
#include <memory>
#include <stdexcept>
#include <map>
#include "debug.h"
#include "win32/controls/control.h"
#include "win32/events/events.h"

class Window : public Control
{
public:
	Window(HINSTANCE hInstance, int nCmdShow, const std::string& className, const std::string& title, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int width = CW_USEDEFAULT, int height = CW_USEDEFAULT);
	~Window();
	// Override to setup buttons, controls, etc.
	virtual void onCreate(HWND hWnd) {};
	void show();
	void registerControl(int menuId, Control* control);
protected:
	std::string className;
	std::string title;
	HINSTANCE hInstance = NULL;
	int nCmdShow,
		x,
		y,
		width,
		height;

private:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMSG, WPARAM wParam, LPARAM lParam);
	std::map<int, Control*> controls;

	void messageLoop();
};

#endif