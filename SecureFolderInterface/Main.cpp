#include "Main.h"
#pragma warning(disable: 4996)

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LPCWSTR ClassName = L"SecureFolderMainInstance";
LPCWSTR WindowName = L"Secure Folder";

HINSTANCE myHInstance;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR cmdLine, int cmdShow)
{
	hInstance = hInstance;

	//creation de l'interface
	WNDCLASS winClass = {};

	winClass.lpfnWndProc = WindowProc;
	winClass.hInstance = hInstance;
	winClass.lpszClassName = ClassName;

	RegisterClass(&winClass);

	HWND hwnd = CreateWindowEx(
		WS_EX_ACCEPTFILES, //windows extended styles
		ClassName, //class name
		WindowName, //windows name
		WS_SYSMENU | WS_MINIMIZEBOX, //windows std styles
		DEF_X_POSITION, //x position
		DEF_Y_POSITION, //y position
		DEF_WIDTH, //width
		DEF_HEIGHT, //height
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (hwnd == NULL)
	{
		return EXIT_NO_WINDOW;
	}

	ShowWindow(hwnd, cmdShow);

	MSG msg = { };

	while (GetMessage(&msg, hwnd, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return LastUnhandledError;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT: //paint window
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		
		paint(hwnd, hdc, ps);

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_CREATE: //initialise window
		break; 
	case WM_CLOSE: //event on Alt + F4 or close button
	{
		if (readyToClose())
		{
			DestroyWindow(hwnd);
		}
		else
		{
			MessageBoxA(hwnd, "The application is currently being used.", NULL, MB_OK | MB_ICONSTOP | MB_APPLMODAL | MB_SETFOREGROUND);
		}
		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_DROPFILES: //when file is dragged and dropped in window
	{
		HDROP hDrop = (HDROP) wParam;

		char filePath[_MAX_PATH];
		if (DragQueryFileA(hDrop, 0, filePath, sizeof(filePath)) != 0)
		{
			setTargetPath(filePath, hwnd);
		}

		DragFinish(hDrop);
		return 0;
	}
	case WM_SIZING:
	{
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		onMousePress(hwnd, myHInstance, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case WM_CHAR:
	{
		onKeyPress(wParam);
		InvalidateRect(hwnd, NULL, true);
	}
	default: break;
	}
	//TODO: handle the mouse type
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}