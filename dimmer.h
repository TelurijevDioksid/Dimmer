#include <stdio.h>
#include <windows.h>
#include <commctrl.h>

LRESULT CALLBACK MainWindowProc(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);

void DrawDimmerScreen(void);

void ProcessInput(void);

void DrawTextOnScreen(HWND WindowHandle);

BOOL DimmerIsAlreadyRunning(void);

DWORD CreateMainDimmerWindow(void);

DWORD WINAPI CreateTrackbar(HWND hwndDlg, UINT iMin, UINT iMax, UINT iSelMin, UINT iSelMax);

