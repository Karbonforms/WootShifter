#include "WindowHelper.h"
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>


#include "Controller.h"
#include "juce_gui_basics/detail/juce_WindowingHelpers.h"

using namespace juce;

using detail::WindowingHelpers;

namespace
{
    HWINEVENTHOOK _eventHook = nullptr;
    
    struct EnumData
    {
        DWORD ProcessId;
        HWND Window;
    };

    bool IsAppWindow ( const HWND hwnd )
    {
#ifdef DEBUG
        wchar_t windowTitle[256];
        GetWindowTextW(hwnd, windowTitle, 256);
        auto msg = String(windowTitle);
#endif

        if (!IsWindowVisible(hwnd))
        {
            DBG( msg << " Window not visible\n" );
            return false;
        }

        if (GetWindow(hwnd, GW_OWNER) != nullptr)
        {
#ifdef DEBUG
            msg << " Window has owner\n";
            DBG( msg );
#endif
            return false;
        }

        const LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
        const LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

        if ((style & WS_DISABLED) || (exStyle & WS_EX_TOOLWINDOW))
        {
            DBG( msg << " Window has disabled or toolwindow style\n" );
            return false;
        }

        wchar_t className[256];
        GetClassNameW(hwnd, className, sizeof(className) / sizeof(wchar_t));

        if (wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
            wcscmp(className, L"ApplicationFrameWindow") == 0)
        {
            DBG( msg << " System window class " << className << "\n" );
            return false;
        }

        RECT rect;
        GetWindowRect(hwnd, &rect);
        if (rect.right - rect.left <= 0 || rect.bottom - rect.top <= 0)
        {
            DBG( msg << " Window has zero size\n" );
            return false;
        }
        
        DBG("Accepted window. Title: " + msg);
        
        return true;
    }

    BOOL EnumFunc ( const HWND hwnd, const LPARAM lParam )
    {
        auto& [pid, window] = *reinterpret_cast<EnumData*>(lParam);

        DWORD window_process_id = 0;
        GetWindowThreadProcessId(hwnd, &window_process_id);

        if (window_process_id == pid && IsAppWindow(hwnd))
        {
            window = hwnd;
            return FALSE;
        }

        return TRUE;
    }

    void CALLBACK WinEventProc ( [[maybe_unused]] HWINEVENTHOOK hook
                                         , DWORD event
                                         , HWND hwnd
                                         , [[maybe_unused]] LONG idObject
                                         , [[maybe_unused]] LONG idChild
                                         , [[maybe_unused]] DWORD idEventThread
                                         , [[maybe_unused]] DWORD dwmsEventTime )
    {
        if (hwnd && event == EVENT_SYSTEM_FOREGROUND)
        {
            Controller::getInstance()->handleProfileActivation(hwnd);
        }
    }

    HWND findMainWindow ( DWORD processId )
    {
        EnumData data = {.ProcessId = processId, .Window = nullptr};

        EnumWindows(EnumFunc, reinterpret_cast<LPARAM>(&data));

        return data.Window;
    }
}

String WindowHelper::GetWindowProcessExePath ( HWND handle )
{
    if (handle)
    {
        DWORD processId;
        GetWindowThreadProcessId(handle, &processId);

        HANDLE processHandle = OpenProcess(
                                           PROCESS_QUERY_INFORMATION | PROCESS_VM_READ
                                           , FALSE
                                           , processId);

        if (!processHandle)
        {
            DBG("OpenProcess failed with error: " + String(GetLastError()));

            // Try with fewer access rights
            processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
            if (!processHandle)
            {
                DBG("OpenProcess (limited) failed with error: " + String(GetLastError()));
                return {};
            }
        }

        if (processHandle)
        {
            CHAR filename[MAX_PATH];
            DWORD size = MAX_PATH;

            // Try QueryFullProcessImageNameA first
            if (QueryFullProcessImageNameA(processHandle, 0, filename, &size) == 0)
            {
                DBG("QueryFullProcessImageNameA failed with error: " + String(GetLastError()));
                
                if (GetModuleFileNameEx(processHandle, nullptr, filename, MAX_PATH) <= 0)
                {
                    DBG("GetModuleFileNameEx failed with error: " + String(GetLastError()));
                    
                    CloseHandle(processHandle);
                    return {};
                }
            }
            CloseHandle(processHandle);
            
            DBG(filename);
            
            return filename;
        }
    }

    return {};
}




bool WindowHelper::RegisterWindowEvents ()
{
    _eventHook = SetWinEventHook(
                           EVENT_SYSTEM_FOREGROUND, // Start event
                           EVENT_SYSTEM_FOREGROUND, // End event
                           nullptr, // No DLL injection
                           WinEventProc, // Callback
                           0, // All processes
                           0, // All threads
                           WINEVENT_OUTOFCONTEXT |
                           WINEVENT_SKIPOWNPROCESS
                          );

    return _eventHook != nullptr;
}

void WindowHelper::UnRegisterWindowEvents ()
{
    UnhookWinEvent(_eventHook);
}

bool WindowHelper::IsEventHookValid ()
{
    return _eventHook != nullptr;
}

String WindowHelper::GetActiveWindowPath ()
{
    static HWND previousHandle = nullptr;
    static String savedPath;

    const auto handle = GetForegroundWindow();

    if (handle == nullptr) return {};

    if (handle == previousHandle && previousHandle != nullptr) return savedPath;

    previousHandle = handle;
    savedPath = GetWindowProcessExePath(handle);
    return savedPath;
}

std::vector<WindowHelper::ProcessInfo> WindowHelper::GetProcesses ()
{
    std::vector<ProcessInfo> result;

    const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return result;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(snapshot, &pe32))
    {
        do
        {
            const HWND hwnd = findMainWindow(pe32.th32ProcessID);

            if (!hwnd) continue;

            String path = GetWindowProcessExePath(hwnd);

            if (path.isEmpty()) continue;

            const auto icon = WindowingHelpers::createIconForFile(File(path));

            result.emplace_back(path, icon);
        }
        while (Process32NextW(snapshot, &pe32));
    }

    CloseHandle(snapshot);

    return result;
}


