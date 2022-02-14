#include "..\..\..\include\win32\controls\window.h"

Window::Window(HINSTANCE hInstance, int nCmdShow, const std::string& className, const std::string& title, int x, int y, int width, int height)
	: hInstance(hInstance), nCmdShow(nCmdShow), className(className), x(x), y(y), width(width), height(height), title(title)
{
	hWnd = NULL;
	parentHwnd = NULL;
}

Window::~Window()
{
}

LRESULT Window::WindowProc(HWND hWnd, UINT uMSG, WPARAM wParam, LPARAM lParam)
{
	Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (uMSG)
	{
	case WM_NCCREATE:
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_CREATE:
		window->onCreate(hWnd);
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_BTNFACE + 1));
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_COMMAND:
		switch (HIWORD(wParam)) {
		case BN_CLICKED:
			printDebug("Button click ID: %d\n", LOWORD(wParam));
			for (const auto& val : window->controls) 
			{
				if (val.first == LOWORD(wParam) && val.second != nullptr) {
					val.second->triggerEvent(EventType::CLICK_EVENT);
					break;
				}
			}
			break;
		}
		break;
	}

	return DefWindowProc(hWnd, uMSG, wParam, lParam);
}

void Window::show()
{
	WNDCLASS wc = {};

	wc.lpfnWndProc = Window::WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = className.c_str();
	wc.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	RegisterClass(&wc);

	hWnd = CreateWindowEx(
		0,
		className.c_str(),
		this->title.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		x, y, width, height,
		NULL,
		NULL,
		hInstance,
		this
	);

	if (hWnd == NULL)
	{
		throw std::runtime_error("Couldn't initialize main window");
	}

	ShowWindow(hWnd, nCmdShow);

	messageLoop();
}

void Window::registerControl(int menuId, Control* control)
{
	controls[menuId] = control;
}

void Window::messageLoop()
{
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
