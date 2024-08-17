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
int HotCornerWindowType = 2; // 0=Button, 1=CharmsBar, 2=CharmsButton
int HotCornerType = 3; // 0=TopLeft, 1=TopRight, 2=BottomLeft, 3=BottomRight
int blCmdOrShellExecute = 1; // 0=ShellExecute, 1=cmd
int brCmdOrShellExecute = 0; // 0=ShellExecute, 1=cmd
char lclick[MAX_BUF_LEN] = "start \"\" cmd /c \"dir %SystemDrive% && pause\"";//"::0";//"";// CharmsBar: ""
char rclick[MAX_BUF_LEN] = "winver";//"";//"::1";// CharmsBar: ""
char help[MAX_BUF_LEN] = "";//"Left-click to simulate WIN button, right-click to simulate WIN+D";// CharmsBar: ""
char image[MAX_BUF_LEN] = "Show_desktop.bmp";//"Start.bmp";//"";// CharmsBar: ""
int offset_image_x = 0;
int offset_image_y = -414;//0;//CharmButton: -300/-414, Button and CharmsBar: 0
int ptx_err = 5;
int pty_err = 5;
int windowStyle = WS_EX_TOOLWINDOW|WS_EX_TOPMOST;//128;//0x80=128=WS_EX_TOOLWINDOW,0x8=8=WS_EX_TOPMOST so default is 136
int CharmsType = 0; // 0=Vertical, 1=Horizontal
int CharmsWidth = 114; // 85 in Windows 8.1
int CharmsHeight = 90;
int CharmsRed = 19;
int CharmsGreen = 14;
int CharmsBlue = 18;
int timerPeriod = 100;
#pragma endregion

#pragma region Global variables
int screenWidth = 0, screenHeight = 0, x = 0, y = 0, wx = 0, wy = 0;
HBITMAP hBitmap;
BITMAP bitmap;
char HOT_CORNERS_BUTTON_CLASS_NAME[] = "Hot Corners Button Class";
char HOT_CORNERS_CHARMS_BAR_CLASS_NAME[] = "Hot Corners Charms Bar Class";
char HOT_CORNERS_CHARMS_BUTTON_CLASS_NAME[] = "Hot Corners Charms Button Class";
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

void ClickAction(char* cmd, int bCmdOrShellExecute)
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
		if (bCmdOrShellExecute)
		{
			// Execute the command using cmd.exe
			char cmdparams[MAX_BUF_LEN+3] = "";
			strcat_s(cmdparams, MAX_BUF_LEN+3, "/c ");
			strcat_s(cmdparams, MAX_BUF_LEN+3, cmd);
			ShellExecute(NULL, "open", "cmd.exe", cmdparams, NULL, SW_HIDE);
		}
		else
		{
			// Execute the command using ShellExecute
			ShellExecute(NULL, "open", cmd, NULL, NULL, SW_SHOW);
		}
	}
}

BOOL CALLBACK EnumWindowsHideProc(HWND hwnd, LPARAM lParam)
{
    char class_name[80];
    GetClassName(hwnd, class_name, sizeof(class_name));

    // If the class name matches, hide the window
    if ((strcmp(class_name, HOT_CORNERS_CHARMS_BAR_CLASS_NAME) == 0)||
		(strcmp(class_name, HOT_CORNERS_CHARMS_BUTTON_CLASS_NAME) == 0))
    {
        ShowWindow(hwnd, SW_HIDE);
    }

    return TRUE; // Continue enumeration
}

BOOL CALLBACK EnumWindowsSetTopProc(HWND hwnd, LPARAM lParam)
{
    char class_name[80];
    GetClassName(hwnd, class_name, sizeof(class_name));

    // If the class name matches, set the window to be always on top
    if (strcmp(class_name, HOT_CORNERS_CHARMS_BUTTON_CLASS_NAME) == 0)
    {
		ShowWindow(hwnd, SW_SHOW);
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }

    return TRUE; // Continue enumeration
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
		ClickAction(lclick, blCmdOrShellExecute);
		if (HotCornerWindowType == 0) ShowWindow(hwnd, SW_HIDE);
		break;
	case WM_RBUTTONDOWN:
		ClickAction(rclick, brCmdOrShellExecute);
		if (HotCornerWindowType == 0) ShowWindow(hwnd, SW_HIDE);
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
		switch (HotCornerWindowType)
		{
		case 0:
			ShowWindow(hwnd, SW_HIDE);
			break;
		default:
		{
			// Get the current mouse position
			POINT pt;
			GetCursorPos(&pt);
			if ((CharmsType == 0 && pt.x >= wx && pt.x <= x)||(CharmsType == 1 && pt.y >= wy && pt.y <= y))
			{
				EnumWindows(EnumWindowsHideProc, 0);
			}
			break;
		}
		}
		break;
	}
	case WM_TIMER:
	{
		switch (wParam)
		{
		case IDT_TIMER1:
			// Screen dimensions may have changed
			UpdateWindowPosition();
			MoveWindow(hwnd, x+offset_image_x, y+offset_image_y, wx, wy, TRUE);

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
				ShowWindow(hwnd, SW_SHOW);
				if (HotCornerWindowType == 1) EnumWindows(EnumWindowsSetTopProc, 0);
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

	char CLASS_NAME[80];
	switch (HotCornerWindowType)
	{
	case 2:
		strcpy_s(CLASS_NAME, HOT_CORNERS_CHARMS_BUTTON_CLASS_NAME);
		break;
	case 1:
		strcpy_s(CLASS_NAME, HOT_CORNERS_CHARMS_BAR_CLASS_NAME);
		break;
	default:
		strcpy_s(CLASS_NAME, HOT_CORNERS_BUTTON_CLASS_NAME);
		break;
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
