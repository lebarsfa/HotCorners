// HotCorners.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "HotCorners.h"

#pragma region Defines
#define EXIT_INVALID_PARAMETER 3
#define EXIT_OUT_OF_MEMORY 6

#define MAX_BUF_LEN 1024

#define IDT_TIMER1 1
#pragma endregion 

#pragma region Parameters

// 3 main class of Hot Corners windows: Button, CharmBar and CharmButton

int HotCornerType = 3; // 0=TopLeft, 1=TopRight, 2=BottomLeft, 3=BottomRight
char lclick[MAX_BUF_LEN] = "";//"::0";//"/c start \"\" cmd /c \"dir %SystemDrive% && pause\"";// CharmBar: ""
char rclick[MAX_BUF_LEN] = "";//"::1";//"/c winver";// CharmBar: ""
char help[MAX_BUF_LEN] = "";//"Left-click to simulate WIN button, right-click to simulate WIN+D";// CharmBar: ""
char image[MAX_BUF_LEN] = "";//"Start.bmp";// CharmBar: ""
int offset_image_x = 0;
int offset_image_y = 0;//-300;//CharmButton: -300, Button and CharmBar: 0
int ptx_err = 5;
int pty_err = 5;
int windowStyle = WS_EX_TOOLWINDOW|WS_EX_TOPMOST;//128;//0x80=128=WS_EX_TOOLWINDOW,0x8=8=WS_EX_TOPMOST so default is 136
char CLASS_NAME[] = "Hot Corners Charm Bar Class";//"Hot Corners Button Class";//
int CharmsType = 0; // 0=Vertical, 1=Horizontal
int CharmsWidth = 114; // 85 in Windows 8.1
int CharmsHeight = 90;
int CharmsRed = 19;
int CharmsGreen = 14;
int CharmsBlue = 18;
//int delay = 100;//0;// CharmButton: 100, otherwise 0
int timerPeriod = 100;
#pragma endregion

#pragma region  Global variables
int screenWidth = 0, screenHeight = 0, x = 0, y = 0, wx = 0, wy = 0;
HBITMAP hBitmap;
BITMAP bitmap;
#pragma endregion

void UpdateWindowPosition() 
{
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	x = 0; y = 0;
	if (image != NULL && image[0] != '\0')
	{
		wx = bitmap.bmWidth;
		wy = bitmap.bmHeight;
	}
	else
	{
		if (CharmsType == 1)
		{
			wx = screenWidth;
			wy = CharmsHeight;
		}
		else
		{
			wx = CharmsWidth;
			wy = screenHeight;
		}
	}

	if (HotCornerType == 0)
	{
		x = 0;
		y = 0;
	}
	else if (HotCornerType == 1)
	{
		x = screenWidth - wx;
		y = 0;
	}
	else if (HotCornerType == 2)
	{
		x = 0;
		y = screenHeight - wy;
	}
	else if (HotCornerType == 3)
	{
		x = screenWidth - wx;
		y = screenHeight - wy;
	}
}

void ClickAction(char* cmd)
{
	if (cmd != NULL && cmd[0] == ':' && cmd[1] == ':' && cmd[2] == '0' && cmd[3] == '\0')
	{
		// Simulate WIN key press
		keybd_event(VK_LWIN, 0, 0, 0); // Press the WIN key
		keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0); // Release the WIN key
	}
	else if (cmd != NULL && cmd[0] == ':' && cmd[1] == ':' && cmd[2] == '1' && cmd[3] == '\0')
	{
		// Simulate WIN + D key press to show desktop
		keybd_event(VK_LWIN, 0, 0, 0); // Press the WIN key
		keybd_event('D', 0, 0, 0); // Press the D key
		keybd_event('D', 0, KEYEVENTF_KEYUP, 0); // Release the D key
		keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0); // Release the WIN key
	}
	else if (cmd != NULL && cmd[0] != '\0')
	{
		// Run the command in rclick
		ShellExecute(NULL, "open", "cmd.exe", rclick, NULL, SW_HIDE);
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		if (image != NULL && image[0] != '\0')
		{
			// Create a memory device context
			HDC hMemDC = CreateCompatibleDC(hdc);

			// Select the bitmap object into the memory device context
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

			// Draw the bitmap at the top-left corner
			BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hMemDC, 0, 0, SRCCOPY);

			// Cleanup
			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
		}
		else {
			// Set your desired color
			HBRUSH hBrush = CreateSolidBrush(RGB(CharmsRed, CharmsGreen, CharmsBlue));

			// Fill the client area with color
			RECT rect;
			GetClientRect(hwnd, &rect);
			FillRect(hdc, &rect, hBrush);

			// Cleanup
			DeleteObject(hBrush);
		}

		EndPaint(hwnd, &ps);
		break;
	}
	case WM_LBUTTONDOWN:
		ClickAction(lclick);
		ShowWindow(hwnd, SW_HIDE);
		break;
	case WM_RBUTTONDOWN:
		ClickAction(rclick);
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
	{
		ShowWindow(hwnd, SW_HIDE);

		//
		// To check...
		// 
		//// Get the current mouse position
		//POINT pt;
		//GetCursorPos(&pt);
		//if ((CharmsType == 0 && pt.x >= wx && pt.x <= x)||(CharmsType == 1 && pt.y >= wy && pt.y <= y))
		//{
		//	ShowWindow(hwnd, SW_HIDE);
		//	if (image != NULL && image[0] != '\0')
		//	{
		//		HWND hTargetWnd = FindWindow("Hot Corners Charm Bar Class", NULL);
		//		if (hTargetWnd != NULL)
		//		{
		//			ShowWindow(hTargetWnd, SW_HIDE);
		//		}
		//	}
		//	else
		//	{
		//		HWND hTargetWnd = FindWindow("Hot Corners Button Class", NULL);
		//		if (hTargetWnd != NULL)
		//		{
		//			ShowWindow(hTargetWnd, SW_HIDE);
		//		}
		//	}
		//}
		break;
	}
	case WM_TIMER:
	{
		switch (wParam)
		{
		case IDT_TIMER1:
			// Screen dimensions may have changed
			UpdateWindowPosition();

			// Get the current mouse position
			POINT pt;
			GetCursorPos(&pt);

			// Check if the mouse is in a corner of the screen
			// On Windows 8.1, it also reacts when x==screenWidth and 0<=y<=10, to check when multiple monitors for Charms there seem to be timings and differences depending on the corner...
			if ((HotCornerType == 0 && pt.x >= -ptx_err && pt.x <= ptx_err && pt.y >= -pty_err && pt.y <= pty_err)||
				(HotCornerType == 1 && pt.x >= screenWidth-1-ptx_err && pt.x <= screenWidth-1+ptx_err && pt.y >= -pty_err && pt.y <= pty_err)||
				(HotCornerType == 2 && pt.x >= -ptx_err && pt.x <= ptx_err && pt.y >= screenHeight-1-pty_err && pt.y <= screenHeight-1+pty_err)||
				(HotCornerType == 3 && pt.x >= screenWidth-1-ptx_err && pt.x <= screenWidth-1+ptx_err && pt.y >= screenHeight-1-pty_err && pt.y <= screenHeight-1+pty_err))
			{
				MoveWindow(hwnd, x+offset_image_x, y+offset_image_y, wx, wy, TRUE);
				ShowWindow(hwnd, SW_SHOW);

				//
				// To check...
				// 
				//if (image != NULL && image[0] != '\0')
				//{
				//	// Get the handle to the window of the specific class
				//	HWND hTargetWnd = FindWindow("Hot Corners Charm Bar Class", NULL);
				//	if (hTargetWnd != NULL)
				//	{
				//		// Set your window to be always on top of the target window
				//		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				//	}
				//}

				//if (image != NULL && image[0] != '\0' && delay != 0)
				//{
				//	Sleep(delay); // Delay for Charms bar to appear
				//}
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
#pragma region Parameter validation
	if (HotCornerType < 0 || HotCornerType > 3)
	{
		return EXIT_INVALID_PARAMETER;
	}
#pragma endregion

	if (image != NULL && image[0] != '\0')
	{	
		// Load the bitmap image
		hBitmap = (HBITMAP)LoadImage(NULL, image, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		if (hBitmap == NULL)
		{
			return EXIT_INVALID_PARAMETER;
		}

		// Get the bitmap's dimensions
		GetObject(hBitmap, sizeof(bitmap), &bitmap);
	}

	WNDCLASS wc = { };

	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	UpdateWindowPosition();

	HWND hwnd = CreateWindowEx(windowStyle, CLASS_NAME, "Hot Corners", WS_POPUP,
		x+offset_image_x, y+offset_image_y, wx, wy, NULL, NULL, hInstance, NULL);
	if (hwnd == NULL)
	{
		return EXIT_INVALID_PARAMETER;
	}

	if (help != NULL && help[0] != '\0')
	{
		HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
			WS_POPUP | TTS_ALWAYSTIP,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			hwnd, NULL, hInstance, NULL);
		if (!hwndTip)
		{
			return EXIT_OUT_OF_MEMORY;
		}

		TOOLINFO toolInfo = { 0 };
		toolInfo.cbSize = sizeof(toolInfo);
		toolInfo.hwnd = hwnd;
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.uId = (UINT_PTR)hwnd;
		toolInfo.lpszText = help;
		SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
	}

	SetTimer(hwnd, IDT_TIMER1, (UINT)timerPeriod, (TIMERPROC)NULL);
	
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
