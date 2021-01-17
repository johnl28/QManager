///////////////////////////// Free for use /////////////////////////////////
//
//   Project     : QManager
//   File        : QManager.cpp
//
//   Created By: johnl / ionutcriste28@gmail.com
//
////////////////////////////////////////////////////////////////////////////

#include "Resource.h"
#include "framework.h"
#include "QManager.h"


WCHAR szTitle[MAX_LOADSTRING] = L"DKManager v1.1";               // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];                        // the main window class name

bool endThread = false;

NOTIFYICONDATA nidApp;
HINSTANCE hInst;
HMENU hPopMenu;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

std::map<std::string, std::thread*> threads;
std::map<std::string, HWND> wndMap;
std::map<std::string, int> confingMap;

void InitConfig()
{
    Log("Initializing config file..");

    TCHAR szPath[MAX_PATH];

    if (SUCCEEDED(SHGetFolderPath(NULL,
        CSIDL_PERSONAL | CSIDL_FLAG_CREATE,
        NULL,
        0,
        szPath)))
    {
        PathAppend(szPath, TEXT("QManager"));
        if (CreateDirectory(szPath, NULL) == ERROR_PATH_NOT_FOUND)
        {
            MessageBox(GetWindowPtr("window"), L"Failed to create config file.", szTitle, MB_ICONWARNING);
        }


        PathAppend(szPath, TEXT("config.json"));
        auto hFile = CreateFile(szPath,                // name of the write
            GENERIC_WRITE,          // open for writing
            0,                      // do not share
            NULL,                   // default security
            CREATE_NEW,             // create new file only
            FILE_ATTRIBUTE_NORMAL,  // normal file
            NULL);

        CloseHandle(hFile);
    }
}

void SetCfg(std::string cfgName, int cfg)
{
    confingMap[cfgName] = cfg;
}

int GetCfg(std::string cfgName)
{
    auto it = confingMap.find(cfgName);

    if (it != confingMap.end()) {
        return it->second;
    }

    return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    int argCount = 0;
    auto args = CommandLineToArgvW(GetCommandLine(), &argCount);


#ifdef _DEBUG
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    FILE* pNewStdout = nullptr;
    FILE* pNewStderr = nullptr;
    FILE* pNewStdin = nullptr;

    ::freopen_s(&pNewStdout, "CONOUT$", "w", stdout);
    ::freopen_s(&pNewStderr, "CONOUT$", "w", stderr);
    ::freopen_s(&pNewStdin, "CONIN$", "r", stdin);

    Log("Console enabled!");

    for (auto k = 0; k < argCount; ++k)
    {
        Log(args[k]);
    }
#endif

    auto qManagerWnd = FindWindowExW(GetDesktopWindow(), nullptr, L"QManager", NULL);
    if (qManagerWnd)
    {
        //MessageBox(GetDesktopWindow(), L"There is already an instance opened.", szTitle, MB_ICONWARNING);
        ShowWindow(qManagerWnd, SW_SHOW);
        PostQuitMessage(0);
        return FALSE;
    }

    LoadStringW(hInstance, IDC_QMANAGER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_QMANAGER));


    //RegisterHotKey(
    //    NULL,
    //    1,
    //    MOD_ALT | MOD_NOREPEAT,
    //    0x42);

    if (argCount > 0 && !lstrcmpiW(args[1], L"/Minimized"))
    {
        ShowWindow(GetWindowPtr("window"), SW_HIDE);
        Log("Lol gay");
    }

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

       //if (msg.message == WM_HOTKEY)
       //{
       //    ToggleDesktopIcons();
       //}
    }

    return (int) msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QMANAGER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_QMANAGER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    Log("RegisterClassExW");
    return RegisterClassExW(&wcex);
}


void ToggleStartup()
{
    HKEY hkey = NULL;
    LONG createStatus = RegCreateKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey); //Creates a key     

    wchar_t pBuf[1000] = {0};
    wchar_t pBuf2[1000] = {0};

    GetModuleFileNameW(NULL, pBuf, _MAX_PATH);

    _snwprintf_s(pBuf2, sizeof(pBuf2), L"\"%s\" /Minimized", pBuf);

    Log(pBuf2);

    bool isEnabled = GetCfg("startup");
    if (isEnabled)
    {
        RegDeleteValue(hkey, L"QManager");
    }
    else
    {
        auto res = RegSetValueExW(hkey, L"QManager", 0, REG_SZ, (BYTE*)pBuf2, (sizeof(pBuf2) + 1) * sizeof(wchar_t));

        if (res == ERROR_ACCESS_DENIED)
        {
            MessageBox(GetWindowPtr("window"), L"Failed to create Run registry! Access Denied.", szTitle, MB_ICONWARNING);
            return;
        }
    }

    SETC("startup", !isEnabled);
    Log("Startup toggle function called, status: %d", GetCfg("startup"));
}

void CheckStartup()
{
    HKEY subKey = nullptr;
    LONG result2 = RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ|KEY_WOW64_64KEY, &subKey);

    LONG result = RegQueryValueExA(subKey, "QManager", NULL, NULL, NULL, NULL);
    if (result == ERROR_SUCCESS)
    {
        Log("Startup Enabled.");
        Button_SetCheck(GetWindowPtr("startup_btn"), BST_CHECKED);
        SETC("startup", true);
    }
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable
   

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_SYSMENU,
      CW_USEDEFAULT, 0, 290, 160, nullptr, nullptr, hInstance, nullptr);
   ::SetMenu(hWnd, NULL);
   Log("CreateWindow Background");

   if (!hWnd) {
      return FALSE;
   }

   wndMap["window"] = hWnd;

   InitNotification(hInstance, hWnd);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

#ifdef _DESKTOP_MGR_
   InitDekstopMgr(hWnd);
#endif

   auto startupButton = CreateWindow(
       L"BUTTON",
       L"Startup",
       WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | BS_FLAT,
       170, //x
       15, //y
       70, //w
       26, //h
       hWnd,
       (HMENU)ID_STARTUP_BUTTON, //id
       (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
       NULL);
   SendMessage(startupButton, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);
   wndMap["startup_btn"] = startupButton;

   CheckStartup();

   //auto hwndTrack = CreateWindowEx(
   //    0,                               // no extended styles 
   //    TRACKBAR_CLASS,                  // class name 
   //    L"Trackbar Control",              // title (caption) 
   //    WS_CHILD |
   //    WS_VISIBLE |
   //    TBS_AUTOTICKS |
   //    TBS_ENABLESELRANGE,              // style 
   //    20, 70,                          // position 
   //    200, 20,                         // size 
   //    hWnd,                         // parent window 
   //    (HMENU)ID_TIME_BUTTON,                     // control identifier 
   //    (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),                         // instance 
   //    NULL                             // no WM_CREATE parameter 
   //);


   // Initialize config file
   ToggleAutoHideIcons(hWnd);
   Button_SetCheck(GetWindowPtr("Auto-HideBtn"), BST_CHECKED);
   InitConfig();

   return TRUE;
}



void ClickButton(int btnId, HWND &hWnd)
{

    switch (btnId)
    {
    case ID_ICONS_BUTTON:
        {
            ToggleAutoHideIcons(hWnd);
        }
        break;
    case ID_RECYCLE_BUTTON:
        {
            SHEmptyRecycleBinA(nullptr, nullptr, 1);
        }
        break;
    case ID_STARTUP_BUTTON:
        {
            ToggleStartup();
        }
        break;

    default:
        break;
    }
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
#ifndef _DEBUG
    case WM_CLOSE:
    {
        ShowWindow(hWnd, SW_HIDE);
    }
    break;
#endif
	case WM_USER_SHELLICON:
    {
        switch (LOWORD(lParam))
        {
            case WM_RBUTTONDOWN:
            {
                OpenMenu(hWnd);
                return TRUE;
            }

            case WM_LBUTTONDOWN:
            {
                ShowWindow(hWnd, SW_SHOW);
            }
            break;

        }
    }
	break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        ClickButton(wmId, hWnd);
        switch (wmId)
        {
            case IDM_ABOUT:
            {
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            }
            break;

            case IDM_EXIT:
            {
                DestroyWindow(hWnd);
            }
            break;

            default:
            {
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
    }
    break;


    case WM_DESTROY:
    {
        ToggleDesktopIcons(SW_SHOW); // show icons
        endThread = true; // end all the threads
        Shell_NotifyIcon(NIM_DELETE, &nidApp); //delete the notify icon
        FreeConsole();
        PostQuitMessage(0);
    }
    break;


    default:
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    }
    return 0;
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        case WM_INITDIALOG:
        {
            return (INT_PTR)TRUE;
        }

        case WM_COMMAND:
        {
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
        }
        break;
    }

    return (INT_PTR)FALSE;
}


void Log(const char* format, ...)
{
#ifdef _DEBUG
    char buff[500];
    va_list args;

    va_start(args, format);
    int32_t len = vsnprintf(buff, sizeof(buff), format, args);
    va_end(args);

    std::cout << buff << std::endl;
#endif
}

void Log(LPWSTR format, ...)
{
#ifdef _DEBUG
   //char buff[500];
   //va_list args;
   //
   //va_start(args, format);
   //int32_t len = vsnprintf(buff, sizeof(buff), format, args);
   //va_end(args);

    std::wcout << format << std::endl;
#endif
}

HWND GetWindowPtr(const std::string name)
{
    auto it = wndMap.find(name);
    if (it == wndMap.end()) {
        return nullptr;
    }

    return it->second;
}


