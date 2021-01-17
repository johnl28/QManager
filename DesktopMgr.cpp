#include "framework.h"
#include "QManager.h"


#ifdef _DESKTOP_MGR_
static bool autoHideActive = false;
static uint8_t timeLeft = 0;
static uint8_t wait = 5;


BOOL CALLBACK EnumWindowFunc(HWND wmd, LPARAM param)
{
    HWND shWnd = FindWindowExW(wmd, nullptr, L"SHELLDLL_DefView", NULL);
    HWND sysWnd = FindWindowExW(shWnd, nullptr, L"SysListView32", NULL);
    if (shWnd && sysWnd)
    {
        wndMap["WorkerW"] = wmd;
        wndMap["SHELLDLL_DefView"] = shWnd;
        wndMap["SysListView32"] = sysWnd;
        Log("Found worker window!");
        return FALSE;
    }
    return TRUE;
}

void InitWorkerWnd()
{
    HWND workerWnd = FindWindow(L"Progman", NULL);;
    HWND shWnd = FindWindowExW(workerWnd, nullptr, L"SHELLDLL_DefView", NULL);
    HWND sysWnd = FindWindowExW(shWnd, nullptr, L"SysListView32", NULL);
    if (!sysWnd)
    {
        EnumChildWindows(GetDesktopWindow(), EnumWindowFunc, 0);
        return;
    }

    wndMap["WorkerW"] = workerWnd;
    wndMap["SHELLDLL_DefView"] = shWnd;
    wndMap["SysListView32"] = sysWnd;
    //FindWindowExW(shWnd, nullptr, L"SysListView32", NULL);
}


void ToggleDesktopIcons(int what)
{
    auto shWnd = GetWindowPtr("SHELLDLL_DefView");
    if (!shWnd) {
        InitWorkerWnd();
        return;
    }

   // FindWindowExW(shWnd, nullptr, L"SysListView32", NULL)
    ShowWindow(shWnd, what);
    Log("ToggleDesktopIcons Called Flag %d", what);
}


void SpawnDesktopThreads(HWND& hWnd)
{
    threads["dekstop_update"] = new std::thread(DesktopUpdate, hWnd);
    threads["dekstop_update"]->detach();
    Log("Spawn thread 'dekstop_update'");

    threads["cursor_state"] = new std::thread(CheckCursorState, hWnd);
    threads["cursor_state"]->detach();
    Log("Spawn thread 'cursor_state'");
}

void DesktopUpdate(HWND hWnd)
{
    auto hwndPB = GetWindowPtr("DekstopMgrPB");
    while (!endThread && autoHideActive)
    {
        
        if (hwndPB && timeLeft < wait && IsWindowVisible(hWnd))
        {
            SendMessage(hwndPB, PBM_STEPIT, 0, 0);
        }
        timeLeft += 1; // increase with 1 on each second

        if ((timeLeft == wait))
        {
           ToggleDesktopIcons(SW_HIDE);
        }

        
        Sleep(1000);
    }

}

void CheckCursorState(HWND hWnd)
{

    while (!endThread && autoHideActive)
    {
       if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && GetForegroundWindow() == GetWindowPtr("WorkerW") && timeLeft)
       {
           
           auto hwndPB = GetWindowPtr("DekstopMgrPB");

           if (hwndPB)
           {
               SendMessage(hwndPB, PBM_SETPOS, 0, 0);
           }
           timeLeft = 0;
           ToggleDesktopIcons(SW_SHOW);

           Log("CheckCursorState Thread");
       }
    

        Sleep(10);
    }
   
}

void InitDekstopMgr(HWND &hWnd)
{
    if (!GetWindowPtr("WorkerW"))
    {
        InitWorkerWnd();
    }

    auto parent = CreateWindow(WC_STATIC, L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        10, 10, 
        250, 100,
        hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL);

    auto hwndPB = CreateWindowEx(0, PROGRESS_CLASS, L"DekstopMgrPB",
        WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
        10, 30,
        225, 10,
        parent, (HMENU)0, (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE), NULL);

    SendMessage(hwndPB, PBM_SETRANGE, 0, MAKELPARAM(0, wait));
    SendMessage(hwndPB, PBM_SETSTEP, (WPARAM)1, 0);
    wndMap["DekstopMgrPB"] = hwndPB;


    auto hwndButton = CreateWindow(
        L"BUTTON",
        L"Auto-Hide Desktop Icons",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | BS_FLAT,
        20, //x
        15, //y
        150, //w
        26, //h
        hWnd,
        (HMENU)ID_ICONS_BUTTON, //id
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL);

    SendMessage(hwndButton, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);

    wndMap["Auto-HideBtn"] = hwndButton;

    SpawnDesktopThreads(hWnd);
}

void ToggleAutoHideIcons(HWND &hWnd)
{
    autoHideActive = !autoHideActive;

    if (autoHideActive)
    {
        SpawnDesktopThreads(hWnd);
    }
    else
    {
        delete threads["dekstop_update"];
        threads["dekstop_update"] = nullptr;
        delete threads["cursor_state"];
        threads["dekstop_update"] = nullptr;
        ToggleDesktopIcons(SW_SHOW);
        timeLeft = 0;

        SendMessage(GetWindowPtr("DekstopMgrPB"), PBM_SETPOS, 0, 0);
    }
}
#endif



