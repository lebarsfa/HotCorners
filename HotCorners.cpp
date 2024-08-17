#include <windows.h>
#include <winuser.h>
#include <windowsx.h>
#include <commctrl.h>

#define IDT_TIMER1 1

int HotCornerType = 1;
char lclick[] = "/c start \"\" cmd /c \"dir %SystemDrive% && pause\"";
char rclick[] = "";//"/c winver";
char help[] = "Left-click to simulate WIN button, right-click to simulate WIN+D";
char image[] = "Start.bmp";

int ptx_err = 5;
int pty_err = 5;

int screenWidth = 0;
int screenHeight = 0;

HBITMAP hBitmap;
BITMAP bitmap;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// Create a memory device context
		HDC hMemDC = CreateCompatibleDC(hdc);

		// Select the bitmap object into the memory device context
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

		// Draw the bitmap at the top-left corner
		BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hMemDC, 0, 0, SRCCOPY);

		// Cleanup
		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);

		EndPaint(hwnd, &ps);
		break;
	}
	case WM_LBUTTONDOWN:
		//
		// Action...
		//

		if (lclick != NULL && lclick[0] != '\0')
		{
			// Run the command in lclick
			ShellExecute(NULL, "open", "cmd.exe", lclick, NULL, SW_HIDE);
		}
		else
		{
			// Simulate WIN key press
			keybd_event(VK_LWIN, 0, 0, 0); // Press the WIN key
			keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0); // Release the WIN key
		}

		ShowWindow(hwnd, SW_HIDE);
		break;
	case WM_RBUTTONDOWN:
		//
		// Action...
		//

		if (rclick != NULL && rclick[0] != '\0')
		{
			// Run the command in rclick
			ShellExecute(NULL, "open", "cmd.exe", rclick, NULL, SW_HIDE);
		}
		else
		{
			// Simulate WIN + D key press to show desktop
			keybd_event(VK_LWIN, 0, 0, 0); // Press the WIN key
			keybd_event('D', 0, 0, 0); // Press the D key
			keybd_event('D', 0, KEYEVENTF_KEYUP, 0); // Release the D key
			keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0); // Release the WIN key
		}

		ShowWindow(hwnd, SW_HIDE);
		break;
	case WM_MOUSEMOVE:
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hwnd;
		TrackMouseEvent(&tme);
		break;
	}
	case WM_MOUSELEAVE:
		ShowWindow(hwnd, SW_HIDE);
		break;
	case WM_TIMER:
	{
		switch (wParam)
		{
		case IDT_TIMER1:
			// Get the current mouse position
			POINT pt;
			GetCursorPos(&pt);

			// Check if the mouse is in a corner of the screen
			if ((HotCornerType == 1 && pt.x >= -ptx_err && pt.x <= ptx_err && pt.y >= -pty_err && pt.y <= pty_err)||
				(HotCornerType == 2 && pt.x >= screenWidth-ptx_err && pt.x <= screenWidth+ptx_err && pt.y >= -pty_err && pt.y <= pty_err)||
				(HotCornerType == 3 && pt.x >= -ptx_err && pt.x <= ptx_err && pt.y >= screenHeight-pty_err && pt.y <= screenHeight+pty_err)||
				(HotCornerType == 4 && pt.x >= screenWidth-ptx_err && pt.x <= screenWidth+ptx_err && pt.y >= screenHeight-pty_err && pt.y <= screenHeight+pty_err))
			{
				ShowWindow(hwnd, SW_SHOW);

				MSG msg = { };
				while (GetMessage(&msg, NULL, 0, 0))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			break;
		}
		break;
	}
	case WM_DESTROY:
		KillTimer(hwnd, IDT_TIMER1);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Load the bitmap image
	hBitmap = (HBITMAP)LoadImage(NULL, image, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (hBitmap == NULL)
	{
		return 0;
	}

	// Get the bitmap's dimensions
	GetObject(hBitmap, sizeof(bitmap), &bitmap);

	const char CLASS_NAME[] = "Hot Corners Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	int x = 0, y = 0, wx = bitmap.bmWidth, wy = bitmap.bmHeight;
	if (HotCornerType == 1)
	{
		x = 0;
		y = 0;
	}
	else if (HotCornerType == 2)
	{
		x = screenWidth - wx;
		y = 0;
	}
	else if (HotCornerType == 3)
	{
		x = 0;
		y = screenHeight - wy;
	}
	else if (HotCornerType == 4)
	{
		x = screenWidth - wx;
		y = screenHeight - wy;
	}

	HWND hwnd;
	hwnd = CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST, CLASS_NAME, "Hot Corners", WS_POPUP,
		x, y, wx, wy, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		return 0;
	}

	HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hwnd, NULL, hInstance, NULL);

	if (!hwndTip)
	{
		return FALSE;
	}

	TOOLINFO toolInfo = { 0 };
	toolInfo.cbSize = sizeof(toolInfo);
	toolInfo.hwnd = hwnd;
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId = (UINT_PTR)hwnd;
	toolInfo.lpszText = help;
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

	SetTimer(hwnd, IDT_TIMER1, 100, (TIMERPROC)NULL);
	
	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//if(hBitmap != NULL)
	//{
	//    DeleteObject(hBitmap);
	//    hBitmap = NULL;
	//}

	//DestroyWindow(hwnd);

	return 0;
}
