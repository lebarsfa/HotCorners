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

//
// Double-clicks do not work probably because the window is hidden after the first click...
// 

#pragma region Parameters
int HCWType = 0; // Hot Corner Window Type: 0=Button, 1=CharmsBar, 2=CharmsButton
int HCType = 0; // Hot Corner Type: 0=TopLeft, 1=TopRight, 2=BottomLeft, 3=BottomRight (CharmsButton and CharmsBar: 3)
char lclick[MAX_BUF_LEN] = "::0";//"";//"::1";//"start \"\" cmd /c \"dir %SystemDrive% && pause\"";// Action for a left click. CharmsBar: ""
char rclick[MAX_BUF_LEN] = "::1";//"";//"::0";//"winver";// Action for a right click. CharmsBar: ""
char ldclick[MAX_BUF_LEN] = "";
char rdclick[MAX_BUF_LEN] = "";
int blCmdOrSE = 0; // How to execute the action for a left click. 0=ShellExecute, 1=cmd
int brCmdOrSE = 1; // How to execute the action for a right click. 0=ShellExecute, 1=cmd
int bldCmdOrSE = 0; // How to execute the action for a left double-click. 0=ShellExecute, 1=cmd
int brdCmdOrSE = 1; // How to execute the action for a right double-click. 0=ShellExecute, 1=cmd
char help[MAX_BUF_LEN] = "Left-click to simulate WIN button, right-click to simulate WIN+D";//"";// CharmsBar: ""
char image[MAX_BUF_LEN] = "Start.bmp";//"";//"Show_desktop.bmp";// Image to display. CharmsBar: ""
int imofsx = 0; // Position to display the button or bar
int imofsy = 0; // Position to display the button or bar. CharmsButton: -300/-414, Button and CharmsBar: 0
int xtol = 5; // Tolerance for the mouse position to be considered in the corner
int ytol = 5; // Tolerance for the mouse position to be considered in the corner
int CBType = 0; // 0=Vertical, 1=Horizontal
int CBWidth = 114; // 85 in Windows 8.1
int CBHeight = 90;
int CBRed = 19; // In 0-255 range
int CBGreen = 14; // In 0-255 range
int CBBlue = 18; // In 0-255 range
int windowStyle = WS_EX_TOOLWINDOW|WS_EX_TOPMOST;//128;//0x80=128=WS_EX_TOOLWINDOW,0x8=8=WS_EX_TOPMOST so default is 136
int keyReleaseDelay = 0; // In milliseconds
int timerPeriod = 100; // In milliseconds
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
		if (CBType == 1)
		{
			wx = screenWidth;
			wy = CBHeight;
		}
		else
		{
			wx = CBWidth;
			wy = screenHeight;
		}
	}

	if (HCType == 0)
	{
		x = 0;
		y = 0;
	}
	else if (HCType == 1)
	{
		x = screenWidth - wx;
		y = 0;
	}
	else if (HCType == 2)
	{
		x = 0;
		y = screenHeight - wy;
	}
	else if (HCType == 3)
	{
		x = screenWidth - wx;
		y = screenHeight - wy;
	}
}

void ClickAction(char* cmd, int bCmdOrSE)
{
	if (cmd != NULL && cmd[0] == ':' && cmd[1] == ':' && cmd[2] == '0' && cmd[3] == '\0')
	{
		// Simulate WIN key press
		
		keybd_event(VK_LWIN, 0, 0, 0); // Press the WIN key
		if (keyReleaseDelay > 0) Sleep(keyReleaseDelay);
		keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0); // Release the WIN key

		//// Create a generic keyboard event structure
		//INPUT ip;
		//ip.type = INPUT_KEYBOARD;
		//ip.ki.wScan = 0; // hardware scan code for key
		//ip.ki.time = 0;
		//ip.ki.dwExtraInfo = 0;

		//ip.ki.wVk = VK_LWIN; // virtual-key code for the WIN key
		//ip.ki.dwFlags = 0; // 0 for key press
		//SendInput(1, &ip, sizeof(INPUT));
		//if (keyReleaseDelay > 0) Sleep(keyReleaseDelay);
		//ip.ki.wVk = VK_LWIN; // virtual-key code for the WIN key
		//ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
		//SendInput(1, &ip, sizeof(INPUT));
	}
	else if (cmd != NULL && cmd[0] == ':' && cmd[1] == ':' && cmd[2] == '1' && cmd[3] == '\0')
	{
		// Simulate WIN + D key press to show desktop
		
		keybd_event(VK_LWIN, 0, 0, 0); // Press the WIN key
		keybd_event('D', 0, 0, 0); // Press the D key
		if (keyReleaseDelay > 0) Sleep(keyReleaseDelay);
		keybd_event('D', 0, KEYEVENTF_KEYUP, 0); // Release the D key
		keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0); // Release the WIN key

		//// Create a generic keyboard event structure
		//INPUT ip;
		//ip.type = INPUT_KEYBOARD;
		//ip.ki.wScan = 0; // hardware scan code for key
		//ip.ki.time = 0;
		//ip.ki.dwExtraInfo = 0;

		//ip.ki.wVk = VK_LWIN; // virtual-key code for the WIN key
		//ip.ki.dwFlags = 0; // 0 for key press
		//SendInput(1, &ip, sizeof(INPUT));
		//ip.ki.wVk = 'D'; // virtual-key code for the D key
		//ip.ki.dwFlags = 0; // 0 for key press
		//SendInput(1, &ip, sizeof(INPUT));
		//if (keyReleaseDelay > 0) Sleep(keyReleaseDelay);
		//ip.ki.wVk = 'D'; // virtual-key code for the D key
		//ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
		//SendInput(1, &ip, sizeof(INPUT));
		//ip.ki.wVk = VK_LWIN; // virtual-key code for the WIN key
		//ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
		//SendInput(1, &ip, sizeof(INPUT));
	}
	else if (cmd != NULL && cmd[0] != '\0')
	{
		if (bCmdOrSE)
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

void GetCommandOutput(LPTSTR text)
{
	std::string cmd = "powershell -Command \"[System.Threading.Thread]::CurrentThread.CurrentCulture = 'en-US' ; Get-Date -Format 'HH:mm dddd MMM dd'\" > temp.txt";
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process with no window
	if (!CreateProcess(NULL,   // No module name (use command line)
		(LPSTR)cmd.c_str(),    // Command line
		NULL,                  // Process handle not inheritable
		NULL,                  // Thread handle not inheritable
		FALSE,                 // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,      // No console window
		NULL,                  // Use parent's environment block
		NULL,                  // Use parent's starting directory 
		&si,                   // Pointer to STARTUPINFO structure
		&pi)                   // Pointer to PROCESS_INFORMATION structure
		) {
		return;
	}

	// Wait until child process exits
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	// Read the output from the temp file
	std::wifstream file("temp.txt");
	file.imbue(std::locale(file.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header>));

	std::wstring result;
	std::wstring line;

	while (std::getline(file, line)) {
		result += line;
		result.push_back('\n');
	}

	// Convert std::wstring to std::string
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	std::string narrow_string = converter.to_bytes(result);

	// Convert std::string to char*
	strcpy_s(text, MAX_BUF_LEN, narrow_string.c_str());
}

BOOL CALLBACK EnumWindowsHideProc(HWND hwnd, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
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
	UNREFERENCED_PARAMETER(lParam);
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
#pragma region Text
			//// Set the background color for the text
			//SetBkColor(hdc, RGB(0, 0, 255)); // Change the RGB values to your desired color
			//SetTextColor(hdc, RGB(0, 255, 0));

			//char text[MAX_BUF_LEN];
			//// Get the command output
			//GetCommandOutput((char*)text);
			//TextOut(hdc, 5, 50, text, _tcslen(text));
#pragma endregion
			// Cleanup
			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
		}
		else {
			// Set your desired color
			HBRUSH hBrush = CreateSolidBrush(RGB(CBRed, CBGreen, CBBlue));

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
		if (HCWType == 0) ShowWindow(hwnd, SW_HIDE);
		if (HCWType == 1) EnumWindows(EnumWindowsSetTopProc, 0);
		KillTimer(hwnd, IDT_TIMER1);
		ClickAction(lclick, blCmdOrSE);
		SetTimer(hwnd, IDT_TIMER1, (UINT)timerPeriod, (TIMERPROC)NULL);
		break;
	case WM_RBUTTONDOWN:
		if (HCWType == 0) ShowWindow(hwnd, SW_HIDE);
		if (HCWType == 1) EnumWindows(EnumWindowsSetTopProc, 0);
		KillTimer(hwnd, IDT_TIMER1);
		ClickAction(rclick, brCmdOrSE);
		SetTimer(hwnd, IDT_TIMER1, (UINT)timerPeriod, (TIMERPROC)NULL);
		break;
	case WM_LBUTTONDBLCLK:
		if (HCWType == 0) ShowWindow(hwnd, SW_HIDE);
		if (HCWType == 1) EnumWindows(EnumWindowsSetTopProc, 0);
		KillTimer(hwnd, IDT_TIMER1);
		ClickAction(ldclick, bldCmdOrSE);
		SetTimer(hwnd, IDT_TIMER1, (UINT)timerPeriod, (TIMERPROC)NULL);
		break;
	case WM_RBUTTONDBLCLK:
		if (HCWType == 0) ShowWindow(hwnd, SW_HIDE);
		if (HCWType == 1) EnumWindows(EnumWindowsSetTopProc, 0);
		KillTimer(hwnd, IDT_TIMER1);
		ClickAction(rdclick, brdCmdOrSE);
		SetTimer(hwnd, IDT_TIMER1, (UINT)timerPeriod, (TIMERPROC)NULL);
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
		switch (HCWType)
		{
		case 0:
			ShowWindow(hwnd, SW_HIDE);
			break;
		default:
		{
			// Get the current mouse position
			POINT pt;
			GetCursorPos(&pt);
			if ((CBType == 0 && pt.x >= wx && pt.x <= x)||(CBType == 1 && pt.y >= wy && pt.y <= y))
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
			MoveWindow(hwnd, x+imofsx, y+imofsy, wx, wy, TRUE);

			// Get the current mouse position
			POINT pt;
			GetCursorPos(&pt);

			// Check if the mouse is in a corner of the screen
			// On Windows 8.1, it also reacts when x==screenWidth and 0<=y<=10, to check when multiple monitors for Charms there seem to be timings and differences depending on the corner...
			if ((HCType == 0 && pt.x >= -xtol && pt.x <= xtol && pt.y >= -ytol && pt.y <= ytol)||
				(HCType == 1 && pt.x >= screenWidth-1-xtol && pt.x <= screenWidth-1+xtol && pt.y >= -ytol && pt.y <= ytol)||
				(HCType == 2 && pt.x >= -xtol && pt.x <= xtol && pt.y >= screenHeight-1-ytol && pt.y <= screenHeight-1+ytol)||
				(HCType == 3 && pt.x >= screenWidth-1-xtol && pt.x <= screenWidth-1+xtol && pt.y >= screenHeight-1-ytol && pt.y <= screenHeight-1+ytol))
			{
				ShowWindow(hwnd, SW_SHOW);
				if (HCWType == 1) EnumWindows(EnumWindowsSetTopProc, 0);
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

void ParseParameters()
{
    for (int i = 1; i < __argc; i++)
    {
        std::string arg = __argv[i];
        if (arg.find("--HCWType=") == 0)
            HCWType = std::stoi(arg.substr(3+strlen("HCWType")));
        else if (arg.find("--HCType=") == 0)
            HCType = std::stoi(arg.substr(3+strlen("HCType")));
        else if (arg.find("--lclick=") == 0)
            strncpy_s(lclick, arg.substr(3+strlen("lclick")).c_str(), MAX_BUF_LEN);
        else if (arg.find("--rclick=") == 0)
            strncpy_s(rclick, arg.substr(3+strlen("rclick")).c_str(), MAX_BUF_LEN);
        else if (arg.find("--ldclick=") == 0)
            strncpy_s(ldclick, arg.substr(3+strlen("ldclick")).c_str(), MAX_BUF_LEN);
        else if (arg.find("--rdclick=") == 0)
            strncpy_s(rclick, arg.substr(3+strlen("rdclick")).c_str(), MAX_BUF_LEN);
        else if (arg.find("--blCmdOrSE=") == 0)
            blCmdOrSE = std::stoi(arg.substr(3+strlen("blCmdOrSE")));
        else if (arg.find("--brCmdOrSE=") == 0)
            brCmdOrSE = std::stoi(arg.substr(3+strlen("brCmdOrSE")));
        else if (arg.find("--bldCmdOrSE=") == 0)
            blCmdOrSE = std::stoi(arg.substr(3+strlen("bldCmdOrSE")));
        else if (arg.find("--brdCmdOrSE=") == 0)
            brCmdOrSE = std::stoi(arg.substr(3+strlen("brdCmdOrSE")));
        else if (arg.find("--help=") == 0)
            strncpy_s(help, arg.substr(3+strlen("help")).c_str(), MAX_BUF_LEN);
        else if (arg.find("--image=") == 0)
            strncpy_s(image, arg.substr(3+strlen("image")).c_str(), MAX_BUF_LEN);
        else if (arg.find("--imofsx=") == 0)
            imofsx = std::stoi(arg.substr(3+strlen("imofsx")));
        else if (arg.find("--imofsy=") == 0)
            imofsy = std::stoi(arg.substr(3+strlen("imofsy")));
        else if (arg.find("--xtol=") == 0)
            xtol = std::stoi(arg.substr(3+strlen("xtol")));
        else if (arg.find("--ytol=") == 0)
            ytol = std::stoi(arg.substr(3+strlen("ytol")));
        else if (arg.find("--CBType=") == 0)
            CBType = std::stoi(arg.substr(3+strlen("CBType")));
        else if (arg.find("--CBWidth=") == 0)
            CBWidth = std::stoi(arg.substr(3+strlen("CBWidth")));
        else if (arg.find("--CBHeight=") == 0)
            CBHeight = std::stoi(arg.substr(3+strlen("CBHeight")));
        else if (arg.find("--CBRed=") == 0)
            CBRed = std::stoi(arg.substr(3+strlen("CBRed")));
        else if (arg.find("--CBGreen=") == 0)
            CBGreen = std::stoi(arg.substr(3+strlen("CBGreen")));
        else if (arg.find("--CBBlue=") == 0)
            CBBlue = std::stoi(arg.substr(3+strlen("CBBlue")));
        else if (arg.find("--windowStyle=") == 0)
            windowStyle = std::stoi(arg.substr(3+strlen("windowStyle")));
        else if (arg.find("--keyReleaseDelay=") == 0)
            timerPeriod = std::stoi(arg.substr(3+strlen("keyReleaseDelay")));
        else if (arg.find("--timerPeriod=") == 0)
            timerPeriod = std::stoi(arg.substr(3+strlen("timerPeriod")));
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#pragma region Parameter validation
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);
	ParseParameters();
	if (HCWType < 0 || HCWType > 2)
	{
		return EXIT_INVALID_PARAMETER;
	}
	if (HCType < 0 || HCType > 3)
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
	switch (HCWType)
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
		x+imofsx, y+imofsy, wx, wy, NULL, NULL, hInstance, NULL);
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
