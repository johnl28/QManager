#include "QManager.h"
#include "framework.h"

void InitNotification(HINSTANCE &hInstance, HWND &hWnd)
{
	HICON hMainIcon = LoadIcon(hInstance, (LPCTSTR)MAKEINTRESOURCE(IDI_QMANAGER));

	nidApp.cbSize	= sizeof(NOTIFYICONDATA);
	nidApp.hWnd		= (HWND)hWnd;
	nidApp.uID		= 5;
	nidApp.uFlags	= NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO | NIM_MODIFY;
	nidApp.hIcon	= hMainIcon; // handle of the Icon to be displayed, obtained from LoadIcon 
	nidApp.uCallbackMessage = WM_USER_SHELLICON;

	//wcscpy_s(nidApp.szInfoTitle, _T("This is a title"));
	//HRESULT hr = StringCchCopy(nidApp.szInfo,
	//	ARRAYSIZE(nidApp.szInfo),
	//	TEXT("Your message text goes here."));
	//nidApp.uTimeout = 15000; // in milliseconds

	wcscpy_s(nidApp.szTip, _T("Text in tooltip"));

	LoadString(hInstance, 1, nidApp.szTip, MAX_LOADSTRING);
	Shell_NotifyIcon(NIM_ADD, &nidApp);
}


void OpenMenu(HWND &hWnd)
{
	POINT lpClickPoint;

	UINT uFlag = MF_BYPOSITION | MF_STRING;
	GetCursorPos(&lpClickPoint);
	hPopMenu = CreatePopupMenu();
	InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_ABOUT, _T("About"));
	//InsertMenu(hPopMenu, 0xFFFFFFFF, uFlag, IDM_TEST2, _T("Test 2"));
	//InsertMenu(hPopMenu, 0xFFFFFFFF, uFlag, IDM_TEST1, _T("Test 1"));
	InsertMenu(hPopMenu, 0xFFFFFFFF, MF_SEPARATOR, IDM_SEP, _T("SEP"));
	InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_EXIT, _T("Exit"));

	SetForegroundWindow(hWnd);
	TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y, 0, hWnd, NULL);
}




