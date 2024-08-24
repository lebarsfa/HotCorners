// HotCorners.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "HotCorners.h"

#pragma region Defines
#define EXIT_INVALID_PARAMETER 3
#define EXIT_OUT_OF_MEMORY 6

#define MAX_BUF_LEN 1024

#define IDT_TIMER_HC 1
#define IDT_TIMER_IM 2
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
int imofsx = 0; // Position offset to display the button or bar
int imofsy = 0; // Position offset to display the button or bar. CharmsButton: -300/-414, Button and CharmsBar: 0
double immulwx = 0; // Multiplier of the screen width to add to the button or bar position
double immulhy = 0; // Multiplier of the screen height to add to the button or bar position
int HCofsx = 0; // Position offset for the hot corner
int HCofsy = 0; // Position offset for the hot corner
double HCmulwx = 0; // Multiplier of the screen width to add to the hot corner position
double HCmulhy = 0; // Multiplier of the screen height to add to the hot corner position
int xitol = 5; // Tolerance (x inside the screen) for the mouse position to be considered in the corner
int xotol = 0; // Tolerance (x outside the screen) for the mouse position to be considered in the corner
int yitol = 5; // Tolerance (y inside the screen) for the mouse position to be considered in the corner
int yotol = 0; // Tolerance (y outside the screen) for the mouse position to be considered in the corner
int CBType = 0; // 0=Vertical, 1=Horizontal
int CBWidth = 114; // 86 in Windows 8
int CBHeight = 90;
int BGRed = 17;//19; // Background color in 0-255 range, 17 for Windows 8 Charms bar
int BGGreen = 17;//14; // Background color in 0-255 range, 17 for Windows 8 Charms bar
int BGBlue = 17;//18; // Background color in 0-255 range, 17 for Windows 8 Charms bar
int FRed = 255; // Font color in 0-255 range
int FGreen = 255; // Font color in 0-255 range
int FBlue = 255; // Font color in 0-255 range
int FSize = 30; // Font size
char FType[MAX_BUF_LEN] = "Segoe UI Semilight";
int textx = 5; // Text position
int texty = 5; // Text position
char dispcmd[MAX_BUF_LEN] = "";//"::0";// "powershell -Command \"[System.Threading.Thread]::CurrentThread.CurrentCulture = 'en-US' ; Get-Date -Format 'HH:mm ddd dd MMM'\" > temp.txt";// Hardcoded disposition for date and time if "::0", otherwise command to output if not empty
int pwdelay = 5; // Delay in seconds before shutdown/restart actions "::3" and "::4"
int windowStyle = WS_EX_TOOLWINDOW|WS_EX_TOPMOST;//128;//0x80=128=WS_EX_TOOLWINDOW,0x8=8=WS_EX_TOPMOST so default is 136
int keyReleaseDelay = 0; // In milliseconds
int tmPeriod = 100; // In milliseconds
int imPeriod = 1000; // In milliseconds
#pragma endregion

#pragma region Global variables
int screenWidth = 0, screenHeight = 0, x = 0, y = 0, wx = 0, wy = 0;
HBITMAP hBitmap;
BITMAP bitmap;
char HOT_CORNERS_BUTTON_CLASS_NAME[] = "Hot Corners Button Class";
char HOT_CORNERS_CHARMS_BAR_CLASS_NAME[] = "Hot Corners Charms Bar Class";
char HOT_CORNERS_CHARMS_BUTTON_CLASS_NAME[] = "Hot Corners Charms Button Class";
WCHAR szwFType[MAX_BUF_LEN];
HCURSOR hCursor;
#pragma endregion

int ReloadImage()
{
	if (hBitmap != NULL)
	{
		DeleteObject(hBitmap);
		hBitmap = NULL;
	}

	if (image != NULL && image[0] != '\0')
	{
		hBitmap = (HBITMAP)LoadImage(NULL, image, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		if (hBitmap == NULL)
		{
			// Try again after a short delay
			Sleep(100);
			hBitmap = (HBITMAP)LoadImage(NULL, image, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			if (hBitmap == NULL)
			{
				return EXIT_INVALID_PARAMETER;
			}
		}

		// Get the bitmap's dimensions
		GetObject(hBitmap, sizeof(bitmap), &bitmap);
	}

	return EXIT_SUCCESS;
}

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

void GetCommandOutput(char* cmd, TCHAR* text)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process with no window
	if (!CreateProcess(NULL,   // No module name (use command line)
		(LPSTR)cmd,    // Command line
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

	//lstrcpynW(text, result.c_str(), MAX_BUF_LEN);

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

void ClickAction(char* cmd, int bCmdOrSE)
{
	if (cmd != NULL && cmd[0] == ':' && cmd[1] == ':' && cmd[2] == '0' && cmd[3] == '\0')
	{
		// Simulate WIN key press		
		keybd_event(VK_LWIN, 0, 0, 0); // Press the WIN key
		if (keyReleaseDelay > 0) Sleep(keyReleaseDelay);
		keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0); // Release the WIN key
	}
	else if (cmd != NULL && cmd[0] == ':' && cmd[1] == ':' && cmd[2] == '1' && cmd[3] == '\0')
	{
		// Simulate WIN + D key press to show desktop		
		keybd_event(VK_LWIN, 0, 0, 0); // Press the WIN key
		keybd_event('D', 0, 0, 0); // Press the D key
		if (keyReleaseDelay > 0) Sleep(keyReleaseDelay);
		keybd_event('D', 0, KEYEVENTF_KEYUP, 0); // Release the D key
		keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0); // Release the WIN key
	}
	else if (cmd != NULL && cmd[0] == ':' && cmd[1] == ':' && cmd[2] == '2' && cmd[3] == '\0')
	{
		EnumWindows(EnumWindowsHideProc, 0);
		ShellExecute(NULL, "open", "cmd.exe", "/c shutdown /a", NULL, SW_HIDE);
	}
	else if (cmd != NULL && cmd[0] == ':' && cmd[1] == ':' && cmd[2] == '3' && cmd[3] == '\0')
	{
		char cmdparams[MAX_BUF_LEN+3] = "";
		sprintf_s(cmdparams, MAX_BUF_LEN+3, "/c shutdown /s /t %d", pwdelay);
		ShellExecute(NULL, "open", "cmd.exe", cmdparams, NULL, SW_HIDE);
	}
	else if (cmd != NULL && cmd[0] == ':' && cmd[1] == ':' && cmd[2] == '4' && cmd[3] == '\0')
	{
		char cmdparams[MAX_BUF_LEN+3] = "";
		sprintf_s(cmdparams, MAX_BUF_LEN+3, "/c shutdown /r /t %d", pwdelay);
		ShellExecute(NULL, "open", "cmd.exe", cmdparams, NULL, SW_HIDE);
	}
	else if (cmd != NULL && cmd[0] == ':' && cmd[1] == ':' && cmd[4] == '\0')
	{
		char key[5] = "0x00";
		key[2] = cmd[2];
		key[3] = cmd[3];
		int vk_code = std::stoi(key, nullptr, 16);
		keybd_event((BYTE)vk_code, 0, 0, 0);
		if (keyReleaseDelay > 0) Sleep(keyReleaseDelay);
		keybd_event((BYTE)vk_code, 0, KEYEVENTF_KEYUP, 0);
	}
	else if (cmd != NULL && cmd[0] == ':' && cmd[1] == ':' && cmd[4] == ':' && cmd[7] == '\0')
	{
		char key1[5] = "0x00";
		char key2[5] = "0x00";
		key1[2] = cmd[2];
		key1[3] = cmd[3];
		key2[2] = cmd[5];
		key2[3] = cmd[6];
		int vk_code1 = std::stoi(key1, nullptr, 16);
		int vk_code2 = std::stoi(key2, nullptr, 16);
		keybd_event((BYTE)vk_code1, 0, 0, 0);
		keybd_event((BYTE)vk_code2, 0, 0, 0);
		if (keyReleaseDelay > 0) Sleep(keyReleaseDelay);
		keybd_event((BYTE)vk_code2, 0, KEYEVENTF_KEYUP, 0);
		keybd_event((BYTE)vk_code1, 0, KEYEVENTF_KEYUP, 0);
	}
	else if (cmd != NULL && cmd[0] == ':' && cmd[1] == ':' && cmd[4] == ':' && cmd[7] == ':' && cmd[10] == '\0')
	{
		char key1[5] = "0x00";
		char key2[5] = "0x00";
		char key3[5] = "0x00";
		key1[2] = cmd[2];
		key1[3] = cmd[3];
		key2[2] = cmd[5];
		key2[3] = cmd[6];
		key3[2] = cmd[8];
		key3[3] = cmd[9];
		int vk_code1 = std::stoi(key1, nullptr, 16);
		int vk_code2 = std::stoi(key2, nullptr, 16);
		int vk_code3 = std::stoi(key3, nullptr, 16);
		keybd_event((BYTE)vk_code1, 0, 0, 0);
		keybd_event((BYTE)vk_code2, 0, 0, 0);
		keybd_event((BYTE)vk_code3, 0, 0, 0);
		if (keyReleaseDelay > 0) Sleep(keyReleaseDelay);
		keybd_event((BYTE)vk_code3, 0, KEYEVENTF_KEYUP, 0);
		keybd_event((BYTE)vk_code2, 0, KEYEVENTF_KEYUP, 0);
		keybd_event((BYTE)vk_code1, 0, KEYEVENTF_KEYUP, 0);
	}
	else if (cmd != NULL && cmd[0] == ':' && cmd[1] == ':' && cmd[4] == ':' && cmd[7] == ':' && cmd[10] == ':' && cmd[13] == '\0')
	{
		char key1[5] = "0x00";
		char key2[5] = "0x00";
		char key3[5] = "0x00";
		char key4[5] = "0x00";
		key1[2] = cmd[2];
		key1[3] = cmd[3];
		key2[2] = cmd[5];
		key2[3] = cmd[6];
		key3[2] = cmd[8];
		key3[3] = cmd[9];
		key4[2] = cmd[11];
		key4[3] = cmd[12];
		int vk_code1 = std::stoi(key1, nullptr, 16);
		int vk_code2 = std::stoi(key2, nullptr, 16);
		int vk_code3 = std::stoi(key3, nullptr, 16);
		int vk_code4 = std::stoi(key4, nullptr, 16);
		keybd_event((BYTE)vk_code1, 0, 0, 0);
		keybd_event((BYTE)vk_code2, 0, 0, 0);
		keybd_event((BYTE)vk_code3, 0, 0, 0);
		keybd_event((BYTE)vk_code4, 0, 0, 0);
		if (keyReleaseDelay > 0) Sleep(keyReleaseDelay);
		keybd_event((BYTE)vk_code4, 0, KEYEVENTF_KEYUP, 0);
		keybd_event((BYTE)vk_code3, 0, KEYEVENTF_KEYUP, 0);
		keybd_event((BYTE)vk_code2, 0, KEYEVENTF_KEYUP, 0);
		keybd_event((BYTE)vk_code1, 0, KEYEVENTF_KEYUP, 0);
	}
	else if (cmd != NULL && cmd[0] == ':' && cmd[1] == ':' && cmd[4] == ':' && cmd[7] == ':' && cmd[10] == ':' && cmd[13] == ':' && cmd[16] == '\0')
	{
		char key1[5] = "0x00";
		char key2[5] = "0x00";
		char key3[5] = "0x00";
		char key4[5] = "0x00";
		char key5[5] = "0x00";
		key1[2] = cmd[2];
		key1[3] = cmd[3];
		key2[2] = cmd[5];
		key2[3] = cmd[6];
		key3[2] = cmd[8];
		key3[3] = cmd[9];
		key4[2] = cmd[11];
		key4[3] = cmd[12];
		key5[2] = cmd[14];
		key5[3] = cmd[15];
		int vk_code1 = std::stoi(key1, nullptr, 16);
		int vk_code2 = std::stoi(key2, nullptr, 16);
		int vk_code3 = std::stoi(key3, nullptr, 16);
		int vk_code4 = std::stoi(key4, nullptr, 16);
		int vk_code5 = std::stoi(key5, nullptr, 16);
		keybd_event((BYTE)vk_code1, 0, 0, 0);
		keybd_event((BYTE)vk_code2, 0, 0, 0);
		keybd_event((BYTE)vk_code3, 0, 0, 0);
		keybd_event((BYTE)vk_code4, 0, 0, 0);
		keybd_event((BYTE)vk_code5, 0, 0, 0);
		if (keyReleaseDelay > 0) Sleep(keyReleaseDelay);
		keybd_event((BYTE)vk_code5, 0, KEYEVENTF_KEYUP, 0);
		keybd_event((BYTE)vk_code4, 0, KEYEVENTF_KEYUP, 0);
		keybd_event((BYTE)vk_code3, 0, KEYEVENTF_KEYUP, 0);
		keybd_event((BYTE)vk_code2, 0, KEYEVENTF_KEYUP, 0);
		keybd_event((BYTE)vk_code1, 0, KEYEVENTF_KEYUP, 0);
	}
	else if (cmd != NULL && cmd[0] == ':' && cmd[1] == ':' && cmd[4] == ':' && cmd[7] == ':' && cmd[10] == ':' && cmd[13] == ':' && cmd[16] == ':' && cmd[19] == '\0')
	{
		char key1[5] = "0x00";
		char key2[5] = "0x00";
		char key3[5] = "0x00";
		char key4[5] = "0x00";
		char key5[5] = "0x00";
		char key6[5] = "0x00";
		key1[2] = cmd[2];
		key1[3] = cmd[3];
		key2[2] = cmd[5];
		key2[3] = cmd[6];
		key3[2] = cmd[8];
		key3[3] = cmd[9];
		key4[2] = cmd[11];
		key4[3] = cmd[12];
		key5[2] = cmd[14];
		key5[3] = cmd[15];
		key6[2] = cmd[17];
		key6[3] = cmd[18];
		int vk_code1 = std::stoi(key1, nullptr, 16);
		int vk_code2 = std::stoi(key2, nullptr, 16);
		int vk_code3 = std::stoi(key3, nullptr, 16);
		int vk_code4 = std::stoi(key4, nullptr, 16);
		int vk_code5 = std::stoi(key5, nullptr, 16);
		int vk_code6 = std::stoi(key6, nullptr, 16);
		keybd_event((BYTE)vk_code1, 0, 0, 0);
		keybd_event((BYTE)vk_code2, 0, 0, 0);
		keybd_event((BYTE)vk_code3, 0, 0, 0);
		keybd_event((BYTE)vk_code4, 0, 0, 0);
		keybd_event((BYTE)vk_code5, 0, 0, 0);
		keybd_event((BYTE)vk_code6, 0, 0, 0);
		if (keyReleaseDelay > 0) Sleep(keyReleaseDelay);
		keybd_event((BYTE)vk_code6, 0, KEYEVENTF_KEYUP, 0);
		keybd_event((BYTE)vk_code5, 0, KEYEVENTF_KEYUP, 0);
		keybd_event((BYTE)vk_code4, 0, KEYEVENTF_KEYUP, 0);
		keybd_event((BYTE)vk_code3, 0, KEYEVENTF_KEYUP, 0);
		keybd_event((BYTE)vk_code2, 0, KEYEVENTF_KEYUP, 0);
		keybd_event((BYTE)vk_code1, 0, KEYEVENTF_KEYUP, 0);
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
			if (dispcmd != NULL && dispcmd[0] == ':' && dispcmd[1] == ':' && dispcmd[2] == '0' && dispcmd[3] == '\0')
			{
				SYSTEM_POWER_STATUS sps;
				int batlevel = 0;
				TCHAR szBatLevel[9];
				SYSTEMTIME st;
				WCHAR szTime[9];
				WCHAR szDayOfWeek[80];
				WCHAR szDate[9];

				if (GetSystemPowerStatus(&sps)) batlevel = static_cast<int>(sps.BatteryLifePercent); else batlevel = -1;
				sprintf_s(szBatLevel, 9, "%d%%", batlevel);

				GetLocalTime(&st);
				GetTimeFormatW(LOCALE_USER_DEFAULT, 0, &st, L"HH':'mm", szTime, 9);
				GetDateFormatW(LOCALE_USER_DEFAULT, 0, &st, L"ddd", szDayOfWeek, 80);
				GetDateFormatW(LOCALE_USER_DEFAULT, 0, &st, L"dd MMM", szDate, 9);

				SetBkColor(hdc, RGB(BGRed, BGGreen, BGBlue));
				SetTextColor(hdc, RGB(FRed, FGreen, FBlue));
				HFONT hFont;
				hFont = CreateFont(45, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T(FType));
				if (hFont)
				{
					SelectObject(hdc, hFont);
					if (sps.ACLineStatus == 0)
					{
						// Running on battery power
						if ((batlevel >= 0)&&(batlevel <= 100))
						{
							TextOut(hdc, 25, 30, szBatLevel, (int)_tcslen(szBatLevel));
						}
						// Covers plug icon
						TextOut(hdc, 27, 30, _T("  "), (int)_tcslen("  "));
						TextOut(hdc, 27, 65, _T("  "), (int)_tcslen("  "));
					}
					else if (sps.ACLineStatus == 1)
					{
						// Plugged in
						if ((batlevel >= 0)&&(batlevel <= 100))
						{
							TextOut(hdc, 25, 30, szBatLevel, (int)_tcslen(szBatLevel));
						}
						else
						{
							// Covers battery icon
							TextOut(hdc, 48, 30, _T("  "), (int)_tcslen("  "));
							TextOut(hdc, 48, 65, _T("  "), (int)_tcslen("  "));
						}
					}
					else
					{
						// Unknown power source
						// Covers plug and battery icons
						TextOut(hdc, 25, 30, _T("      "), (int)_tcslen("      "));
						TextOut(hdc, 25, 65, _T("      "), (int)_tcslen("      "));
					}
					DeleteObject(hFont);
				}
				//SetBkColor(hdc, RGB(BGRed, BGGreen, BGBlue));
				//SetTextColor(hdc, RGB(FRed, FGreen, FBlue));
				hFont = CreateFontW(110, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, szwFType);
				if (hFont)
				{
					SelectObject(hdc, hFont);
					TextOutW(hdc, 100, 10, szTime, (int)wcslen(szTime));
					DeleteObject(hFont);
				}
				hFont = CreateFontW(45, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, szwFType);
				if (hFont)
				{
					SelectObject(hdc, hFont);
					TextOutW(hdc, 310, 20, szDayOfWeek, (int)wcslen(szDayOfWeek));
					TextOutW(hdc, 310, 65, szDate, (int)wcslen(szDate));
					DeleteObject(hFont);
				}
			}
			else if (dispcmd[0] != '\0')
			{
				TCHAR text[MAX_BUF_LEN];
				text[0] = 0;
				GetCommandOutput(dispcmd, text);
				SetBkColor(hdc, RGB(BGRed, BGGreen, BGBlue));
				SetTextColor(hdc, RGB(FRed, FGreen, FBlue));
				HFONT hFont = CreateFont(FSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T(FType));
				if (hFont)
				{
					SelectObject(hdc, hFont);
					TextOut(hdc, textx, texty, text, (int)_tcslen(text));
					DeleteObject(hFont);
				}
			}
#pragma endregion
			// Cleanup
			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
		}
		else {
			// Set your desired color
			HBRUSH hBrush = CreateSolidBrush(RGB(BGRed, BGGreen, BGBlue));

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
		KillTimer(hwnd, IDT_TIMER_HC);
		ClickAction(lclick, blCmdOrSE);
		SetTimer(hwnd, IDT_TIMER_HC, (UINT)tmPeriod, (TIMERPROC)NULL);
		break;
	case WM_RBUTTONDOWN:
		if (HCWType == 0) ShowWindow(hwnd, SW_HIDE);
		if (HCWType == 1) EnumWindows(EnumWindowsSetTopProc, 0);
		KillTimer(hwnd, IDT_TIMER_HC);
		ClickAction(rclick, brCmdOrSE);
		SetTimer(hwnd, IDT_TIMER_HC, (UINT)tmPeriod, (TIMERPROC)NULL);
		break;
	case WM_LBUTTONDBLCLK:
		if (HCWType == 0) ShowWindow(hwnd, SW_HIDE);
		if (HCWType == 1) EnumWindows(EnumWindowsSetTopProc, 0);
		KillTimer(hwnd, IDT_TIMER_HC);
		ClickAction(ldclick, bldCmdOrSE);
		SetTimer(hwnd, IDT_TIMER_HC, (UINT)tmPeriod, (TIMERPROC)NULL);
		break;
	case WM_RBUTTONDBLCLK:
		if (HCWType == 0) ShowWindow(hwnd, SW_HIDE);
		if (HCWType == 1) EnumWindows(EnumWindowsSetTopProc, 0);
		KillTimer(hwnd, IDT_TIMER_HC);
		ClickAction(rdclick, brdCmdOrSE);
		SetTimer(hwnd, IDT_TIMER_HC, (UINT)tmPeriod, (TIMERPROC)NULL);
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
			POINT pt;
			GetCursorPos(&pt);
			if ((CBType == 0 && pt.x+HCofsx+(int)(HCmulwx*screenWidth) >= wx && pt.x+HCofsx+(int)(HCmulwx*screenWidth) <= x)||
				(CBType == 1 && pt.y+HCofsy+(int)(HCmulhy*screenHeight) >= wy && pt.y+HCofsy+(int)(HCmulhy*screenHeight) <= y))
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
		case IDT_TIMER_HC:
			// Screen dimensions may have changed
			UpdateWindowPosition();
			MoveWindow(hwnd, x+imofsx+(int)(immulwx*screenWidth), y+imofsy+(int)(immulhy*screenHeight), wx, wy, TRUE);

			// Get the current mouse position
			POINT pt;
			GetCursorPos(&pt);

			// Check if the mouse is in a corner of the screen
			// On Windows 8.1, it also reacts when x==screenWidth and 0<=y<=10, to check when multiple monitors for Charms there seem to be timings and differences depending on the corner...
			if ((HCType == 0 && pt.x+HCofsx+(int)(HCmulwx*screenWidth) >= -xotol && pt.x+HCofsx+(int)(HCmulwx*screenWidth) <= xitol && pt.y+HCofsy+(int)(HCmulhy*screenHeight) >= -yotol && pt.y+HCofsy+(int)(HCmulhy*screenHeight) <= yitol)||
				(HCType == 1 && pt.x+HCofsx+(int)(HCmulwx*screenWidth) >= screenWidth-1-xitol && pt.x+HCofsx+(int)(HCmulwx*screenWidth) <= screenWidth-1+xotol && pt.y+HCofsy+(int)(HCmulhy*screenHeight) >= -yotol && pt.y+HCofsy+(int)(HCmulhy*screenHeight) <= yitol)||
				(HCType == 2 && pt.x+HCofsx+(int)(HCmulwx*screenWidth) >= -xotol && pt.x+HCofsx+(int)(HCmulwx*screenWidth) <= xitol && pt.y+HCofsy+(int)(HCmulhy*screenHeight) >= screenHeight-1-yitol && pt.y+HCofsy+(int)(HCmulhy*screenHeight) <= screenHeight-1+yotol)||
				(HCType == 3 && pt.x+HCofsx+(int)(HCmulwx*screenWidth) >= screenWidth-1-xitol && pt.x+HCofsx+(int)(HCmulwx*screenWidth) <= screenWidth-1+xotol && pt.y+HCofsy+(int)(HCmulhy*screenHeight) >= screenHeight-1-yitol && pt.y+HCofsy+(int)(HCmulhy*screenHeight) <= screenHeight-1+yotol))
			{
				ShowWindow(hwnd, SW_SHOW);
				if (HCWType == 1) EnumWindows(EnumWindowsSetTopProc, 0);
			}
			break;
		case IDT_TIMER_IM:
			if (IsWindowVisible(hwnd))
			{
				ReloadImage();
				InvalidateRect(hwnd, NULL, TRUE);
			}
			break;
		}
		break;
	}
	case WM_DESTROY:
		KillTimer(hwnd, IDT_TIMER_HC);
		if (imPeriod > 0) KillTimer(hwnd, IDT_TIMER_IM);
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
            strncpy_s(rdclick, arg.substr(3+strlen("rdclick")).c_str(), MAX_BUF_LEN);
        else if (arg.find("--blCmdOrSE=") == 0)
            blCmdOrSE = std::stoi(arg.substr(3+strlen("blCmdOrSE")));
        else if (arg.find("--brCmdOrSE=") == 0)
            brCmdOrSE = std::stoi(arg.substr(3+strlen("brCmdOrSE")));
        else if (arg.find("--bldCmdOrSE=") == 0)
            bldCmdOrSE = std::stoi(arg.substr(3+strlen("bldCmdOrSE")));
        else if (arg.find("--brdCmdOrSE=") == 0)
            brdCmdOrSE = std::stoi(arg.substr(3+strlen("brdCmdOrSE")));
        else if (arg.find("--help=") == 0)
            strncpy_s(help, arg.substr(3+strlen("help")).c_str(), MAX_BUF_LEN);
        else if (arg.find("--image=") == 0)
            strncpy_s(image, arg.substr(3+strlen("image")).c_str(), MAX_BUF_LEN);
        else if (arg.find("--imofsx=") == 0)
            imofsx = std::stoi(arg.substr(3+strlen("imofsx")));
        else if (arg.find("--imofsy=") == 0)
            imofsy = std::stoi(arg.substr(3+strlen("imofsy")));
        else if (arg.find("--immulwx=") == 0)
            immulwx = std::stod(arg.substr(3+strlen("immulwx")));
        else if (arg.find("--immulhy=") == 0)
            immulhy = std::stod(arg.substr(3+strlen("immulhy")));
        else if (arg.find("--HCofsx=") == 0)
            HCofsx = std::stoi(arg.substr(3+strlen("HCofsx")));
        else if (arg.find("--HCofsy=") == 0)
            HCofsy = std::stoi(arg.substr(3+strlen("HCofsy")));
        else if (arg.find("--HCmulwx=") == 0)
            HCmulwx = std::stod(arg.substr(3+strlen("HCmulwx")));
        else if (arg.find("--HCmulhy=") == 0)
            HCmulhy = std::stod(arg.substr(3+strlen("HCmulhy")));
        else if (arg.find("--xitol=") == 0)
            xitol = std::stoi(arg.substr(3+strlen("xitol")));
        else if (arg.find("--xotol=") == 0)
            xotol = std::stoi(arg.substr(3+strlen("xotol")));
        else if (arg.find("--yitol=") == 0)
            yitol = std::stoi(arg.substr(3+strlen("yitol")));
        else if (arg.find("--yotol=") == 0)
            yotol = std::stoi(arg.substr(3+strlen("yotol")));
        else if (arg.find("--CBType=") == 0)
            CBType = std::stoi(arg.substr(3+strlen("CBType")));
        else if (arg.find("--CBWidth=") == 0)
            CBWidth = std::stoi(arg.substr(3+strlen("CBWidth")));
        else if (arg.find("--CBHeight=") == 0)
            CBHeight = std::stoi(arg.substr(3+strlen("CBHeight")));
        else if (arg.find("--BGRed=") == 0)
            BGRed = std::stoi(arg.substr(3+strlen("BGRed")));
        else if (arg.find("--BGGreen=") == 0)
            BGGreen = std::stoi(arg.substr(3+strlen("BGGreen")));
        else if (arg.find("--BGBlue=") == 0)
            BGBlue = std::stoi(arg.substr(3+strlen("BGBlue")));
        else if (arg.find("--FRed=") == 0)
            FRed = std::stoi(arg.substr(3+strlen("FRed")));
        else if (arg.find("--FGreen=") == 0)
            FGreen = std::stoi(arg.substr(3+strlen("FGreen")));
        else if (arg.find("--FBlue=") == 0)
            FBlue = std::stoi(arg.substr(3+strlen("FBlue")));
        else if (arg.find("--FSize=") == 0)
            FSize = std::stoi(arg.substr(3+strlen("FSize")));
        else if (arg.find("--FType=") == 0)
            strncpy_s(FType, arg.substr(3+strlen("FType")).c_str(), MAX_BUF_LEN);
        else if (arg.find("--textx=") == 0)
            textx = std::stoi(arg.substr(3+strlen("textx")));
        else if (arg.find("--texty=") == 0)
            texty = std::stoi(arg.substr(3+strlen("texty")));
        else if (arg.find("--dispcmd=") == 0)
            strncpy_s(dispcmd, arg.substr(3+strlen("dispcmd")).c_str(), MAX_BUF_LEN);
        else if (arg.find("--pwdelay=") == 0)
            pwdelay = std::stoi(arg.substr(3+strlen("pwdelay")));
        else if (arg.find("--windowStyle=") == 0)
            windowStyle = std::stoi(arg.substr(3+strlen("windowStyle")));
        else if (arg.find("--keyReleaseDelay=") == 0)
            keyReleaseDelay = std::stoi(arg.substr(3+strlen("keyReleaseDelay")));
        else if (arg.find("--tmPeriod=") == 0)
            tmPeriod = std::stoi(arg.substr(3+strlen("tmPeriod")));
        else if (arg.find("--imPeriod=") == 0)
            imPeriod = std::stoi(arg.substr(3+strlen("imPeriod")));
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
#pragma region Parameter processing
	int szwFTypeLen = MultiByteToWideChar(CP_ACP, 0, FType, (int)strlen(FType), NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, FType, (int)strlen(FType), szwFType, szwFTypeLen);
	szwFType[szwFTypeLen] = L'\0';
#pragma endregion

	if (ReloadImage() != EXIT_SUCCESS)
	{
		return EXIT_INVALID_PARAMETER;
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
		x+imofsx+(int)(immulwx*screenWidth), y+imofsy+(int)(immulhy*screenHeight), wx, wy, NULL, NULL, hInstance, NULL);
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

	hCursor = LoadCursor(NULL, IDC_ARROW);
	if (hCursor == NULL)
	{
		return EXIT_INVALID_PARAMETER;
	}
	SetCursor(hCursor);
	ShowCursor(TRUE);

	SetTimer(hwnd, IDT_TIMER_HC, (UINT)tmPeriod, (TIMERPROC)NULL);
	if (imPeriod > 0) SetTimer(hwnd, IDT_TIMER_IM, (UINT)imPeriod, (TIMERPROC)NULL);
	
	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (hBitmap != NULL)
	{
	    DeleteObject(hBitmap);
	    hBitmap = NULL;
	}

	//DestroyWindow(hwnd);

	return 0;
}
