#include "dimmer.h"

BOOL gDimmerIsRunning;
HWND gDimmerWindowHandle;
HWND gTrackpadWindowHandle;
int gDimmerAmount;
MONITORINFO gMonitorInfo;

int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PreviousInstance, PSTR CommandLine, int CommandShow)
{
    MSG Message = { 0 };

    if (DimmerIsAlreadyRunning() == TRUE) {
        MessageBoxA(NULL, "Dimmer is already running!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        goto EXIT;
    }

    if (CreateMainDimmerWindow() != ERROR_SUCCESS) {
        MessageBoxA(NULL, "Failed to create window!", "Error!", MB_ICONERROR | MB_OK);
        goto EXIT;
	}

    gDimmerIsRunning = TRUE;
    while (gDimmerIsRunning == TRUE) {
        while (PeekMessageA(&Message, gDimmerWindowHandle, 0, 0, PM_REMOVE)) {
            DispatchMessageA(&Message);
        }
        Sleep(1);
    }
    
EXIT:
    return 0;
}

LRESULT CALLBACK MainWindowProc(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT result = 0;

    switch (Message) {
        case WM_CLOSE:
            gDimmerIsRunning = FALSE;
            PostQuitMessage(0);
            break;
        case WM_HSCROLL:
            gDimmerAmount = (int)SendMessageA(gTrackpadWindowHandle, TBM_GETPOS, 0, 0);
            DrawTextOnScreen(gTrackpadWindowHandle);
            if (SetLayeredWindowAttributes(
                gDimmerWindowHandle,
                RGB(0, 0, 0),
                (int)((gDimmerAmount / 100.0f) * 255),
                LWA_ALPHA) == FALSE) {
                result = GetLastError();
                MessageBoxA(NULL, "Failed to set transparency!", "Error!", MB_ICONEXCLAMATION | MB_OK);
            }
            break;
        default:
            result = DefWindowProcA(WindowHandle, Message, WParam, LParam);
    }

    return result;
}

DWORD CreateMainDimmerWindow(void)
{
    DWORD result = ERROR_SUCCESS;

    WNDCLASSEXA WindowClass = { 0 };
    WindowClass.cbSize = sizeof(WNDCLASSEXA);
    WindowClass.style = 0;
    WindowClass.lpfnWndProc = MainWindowProc;
    WindowClass.cbClsExtra = 0;
    WindowClass.cbWndExtra = 0;
    WindowClass.hInstance = GetModuleHandleA(NULL);
    WindowClass.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    WindowClass.hIconSm = LoadIconA(NULL, IDI_APPLICATION);
    WindowClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    WindowClass.lpszMenuName = NULL;
    WindowClass.lpszClassName = "Dimmer";

    if (RegisterClassExA(&WindowClass) == 0) {
        result = GetLastError();
        MessageBoxA(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        goto EXIT;
    }

    gDimmerWindowHandle = CreateWindowExA(
        0,
        "Dimmer",
        "Dimmer",
        WS_BORDER,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        GetModuleHandleA(NULL),
        NULL
    );

    if (gDimmerWindowHandle == NULL) {
        result = GetLastError();
        MessageBoxA(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        goto EXIT;
    }

    gMonitorInfo.cbSize = sizeof(MONITORINFO);
    if (GetMonitorInfoA(MonitorFromWindow(gDimmerWindowHandle, MONITOR_DEFAULTTOPRIMARY), &gMonitorInfo) == FALSE) {
        result = ERROR_INVALID_DATA;
        MessageBoxA(NULL, "Failed to get monitor info!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        goto EXIT;
    }
    
    SetWindowLongA(gDimmerWindowHandle, GWL_EXSTYLE, WS_EX_LAYERED);

    SetWindowPos(
        gDimmerWindowHandle,
        HWND_TOP,
        gMonitorInfo.rcMonitor.left,
        gMonitorInfo.rcMonitor.top,
        gMonitorInfo.rcMonitor.right - gMonitorInfo.rcMonitor.left,
        gMonitorInfo.rcMonitor.bottom - gMonitorInfo.rcMonitor.top,    
        SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

    ShowWindow(gDimmerWindowHandle, SW_SHOW);

    if (SetLayeredWindowAttributes(gDimmerWindowHandle, RGB(0, 0, 0), 255, LWA_ALPHA) == FALSE) {
        result = GetLastError();
        MessageBoxA(NULL, "Failed to set transparency!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        goto EXIT;
    }

    gTrackpadWindowHandle = CreateWindowExA( 
        0,
        TRACKBAR_CLASS,
        "Trackbar Control",
        WS_CHILD | WS_VISIBLE,
        200,
        100,
        200,
        30,
        gDimmerWindowHandle,
        NULL,
        GetModuleHandleA(NULL),
        NULL
    );
        
    if (gTrackpadWindowHandle == NULL) {
        result = GetLastError();
        MessageBoxA(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        goto EXIT;
    }

    SetWindowLongA(gTrackpadWindowHandle, GWL_EXSTYLE, WS_CHILD);
    DrawTextOnScreen(gTrackpadWindowHandle);
EXIT:
	return result;
}

void DrawTextOnScreen(HWND WindowHandle)
{
    HDC hdc = GetDC(WindowHandle);
    char buffer[32] = { 0 };
    RECT rect;
    GetClientRect(WindowHandle, &rect);
    rect.top = 50;
    rect.left = 10;
    
    sprintf(buffer, "Dimmer amount: 100");
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawTextA(hdc, buffer, -1, &rect, DT_SINGLELINE);
    
    sprintf(buffer, "Dimmer amount: %d", gDimmerAmount);
    SetTextColor(hdc, RGB(0, 0, 0));
    DrawTextA(hdc, buffer, -1, &rect, DT_SINGLELINE);

    ReleaseDC(WindowHandle, hdc);
}

BOOL DimmerIsAlreadyRunning(void)
{
    BOOL result = FALSE;
	CreateMutexA(NULL, FALSE, "dimmer_mutex");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		result = TRUE;
	}
	return result;
}

