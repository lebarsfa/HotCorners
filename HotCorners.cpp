#include <windows.h>
#include <winuser.h>
#include <windowsx.h>

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

		keybd_event(VK_LWIN, 0, 0, 0); // Press the WIN key
		keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0); // Release the WIN key

		ShowWindow(hwnd, SW_HIDE);
		PostQuitMessage(0);
		break;
	case WM_RBUTTONDOWN:
		//
		// Action...
		//

		// Simulate WIN + D key press to show desktop
		keybd_event(VK_LWIN, 0, 0, 0); // Press the WIN key
		keybd_event('D', 0, 0, 0); // Press the D key
		keybd_event('D', 0, KEYEVENTF_KEYUP, 0); // Release the D key
		keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0); // Release the WIN key

		ShowWindow(hwnd, SW_HIDE);
		PostQuitMessage(0);
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
		PostQuitMessage(0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Load the bitmap image
	hBitmap = (HBITMAP)LoadImage(NULL, "Start.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

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

	HWND hwnd = CreateWindowEx(WS_EX_TOOLWINDOW, CLASS_NAME, "Hot Corners", WS_POPUP,
		0, 0, bitmap.bmWidth, bitmap.bmHeight, NULL, NULL, hInstance, NULL);
	//HWND hwnd = CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST, CLASS_NAME, "Hot Corners", WS_POPUP,
	//	0, screenHeight-bitmap.bmHeight, bitmap.bmWidth, bitmap.bmHeight, NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		return 0;
	}

	for (;;)
	{
		// Get the current mouse position
		POINT pt;
		GetCursorPos(&pt);

		// Check if the mouse is in the top-left corner of the screen
		if (pt.x <= 5 && pt.y <= 5)
			//if (pt.x <= 5 && pt.y >= screenHeight-5)
		{
			ShowWindow(hwnd, SW_SHOW);

			MSG msg = { };
			while (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		Sleep(100);
	}

	//if(hBitmap != NULL)
	//{
	//    DeleteObject(hBitmap);
	//    hBitmap = NULL;
	//}

	//DestroyWindow(hwnd);

	return 0;
}
