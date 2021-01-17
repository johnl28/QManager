#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <ShlObj.h>     // Shell API
#include <atlcomcli.h>  // CComPtr & Co.
#include <string> 
#include <iostream> 
#include <system_error>
#include <shellapi.h>
#include <shlwapi.h>
#include <WinUser.h>
#include <vector>
#include <sysinfoapi.h>
#include <map>
#include <thread>
#include <dwmapi.h>
#include <Uxtheme.h>
#include <strsafe.h>
#include <windowsx.h>

void Log(const char* line, ...);
void Log(LPWSTR format, ...);

extern void Test2222();

//global 
void DesktopUpdate(HWND hWnd);
extern std::map<std::string, std::thread*> threads;
extern std::map<std::string, HWND> wndMap;
HWND GetWindowPtr(const std::string name);
void SetCfg(std::string cfgName, int cfg);
int GetCfg(std::string cfgName);
#define SETC(cname, cf) SetCfg(cname, cf)

// notification
void InitNotification(HINSTANCE& hInstance, HWND& hWnd);
void OpenMenu(HWND& hWnd);

#define _DESKTOP_MGR_
#ifdef _DESKTOP_MGR_
	void ToggleDesktopIcons(int what);
	void ToggleAutoHideIcons(HWND& hWnd);
	void InitDekstopMgr(HWND& hWnd);
	void CheckCursorState(HWND hWnd);
#endif
