#include "WindowHelper.h"

#include <psapi.h>
#include <tlhelp32.h>

using namespace juce;
#include "juce_gui_basics/detail/juce_WindowingHelpers.h"

// static BOOL is_main_window(const HWND handle)
// {   
//     return GetWindow(handle, GW_OWNER) == static_cast<HWND>(nullptr) && IsWindowVisible(handle);
// }
//
// static bool isAppWindow(HWND hwnd)
// {   
//     if (!IsWindowVisible(hwnd))
//         return false;
//
//     // Check if window has no owner (like Task Manager's Apps section)
//     if (GetWindow(hwnd, GW_OWNER) != nullptr)
//         return false;
//
//     // Get window styles
//     LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
//     LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
//
//     // Exclude windows with certain styles that typically indicate system/background windows
//     if ((style & WS_DISABLED) || (exStyle & WS_EX_TOOLWINDOW))
//         return false;
//
//     // Get window class name to filter out system windows
//     wchar_t className[256];
//     GetClassNameW(hwnd, className, sizeof(className)/sizeof(wchar_t));
//
//     // Filter out windows with system class names
//     if (wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
//         wcscmp(className, L"ApplicationFrameWindow") == 0)
//         return false;
//
//     // Additional check: window should have a non-zero size
//     RECT rect;
//     GetWindowRect(hwnd, &rect);
//     if (rect.right - rect.left <= 0 || rect.bottom - rect.top <= 0)
//         return false;
//
//     return true;
// }

static bool isAppWindow(const HWND hwnd)
{
#ifdef DEBUG
    wchar_t windowTitle[256];
    GetWindowTextW(hwnd, windowTitle, 256);
    auto msg = String(windowTitle);
#endif
    
    if (!IsWindowVisible(hwnd))
    {
#ifdef DEBUG
        msg << " Window not visible\n";
        Logger::outputDebugString( msg );
#endif
        return false;
    }

    if (GetWindow(hwnd, GW_OWNER) != nullptr)
    {
#ifdef DEBUG
        msg << " Window has owner\n";
        Logger::outputDebugString( msg );
#endif
        return false;
    }

    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

    if ((style & WS_DISABLED) || (exStyle & WS_EX_TOOLWINDOW))
    {
#ifdef DEBUG
        msg << " Window has disabled or toolwindow style\n";
        Logger::outputDebugString( msg );
#endif
        return false;
    }

    wchar_t className[256];
    GetClassNameW(hwnd, className, sizeof(className)/sizeof(wchar_t));
    
    // // Debug: Print class name
    // wchar_t debugMsg[512];
    // swprintf_s(debugMsg, L"Window class name: %s\n", className);
    // OutputDebugStringW(debugMsg);

    if (wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
        wcscmp(className, L"ApplicationFrameWindow") == 0)
    {
#ifdef DEBUG
        msg << " System window class " << className << "\n";
        Logger::outputDebugString( msg );
#endif
        return false;
    }

    RECT rect;
    GetWindowRect(hwnd, &rect);
    if (rect.right - rect.left <= 0 || rect.bottom - rect.top <= 0)
    {
#ifdef DEBUG
        msg << " Window has zero size\n";
        Logger::outputDebugString( msg );
#endif
        return false;
    }

#ifdef DEBUG
    Logger::outputDebugString("Accepted window. Title: " + msg);
#endif
    
    return true;
}



String WindowHelper::getWindowProcessExePath(const HWND handle)
{
    if (handle)
    {
        DWORD process_id;
        GetWindowThreadProcessId(handle, &process_id);

        HANDLE process_handle = OpenProcess(
                            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ
                            , FALSE
                            , process_id);

        if (!process_handle)
        {
#ifdef DEBUG
            Logger::outputDebugString("OpenProcess failed with error: " + String(GetLastError()));
#endif
 
            // Try with fewer access rights
            process_handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, process_id);
            if (!process_handle)
            {
#ifdef DEBUG
                Logger::outputDebugString("OpenProcess (limited) failed with error: " + String(GetLastError()));
#endif
                return {};
            }
        }
        
        if (process_handle)
        {
            CHAR filename[MAX_PATH];
            DWORD size = MAX_PATH;

            // Try QueryFullProcessImageNameA first
            if (QueryFullProcessImageNameA(process_handle, 0, filename, &size) == 0)
            {
#ifdef DEBUG 
                Logger::outputDebugString("QueryFullProcessImageNameA failed with error: " + String(GetLastError()));
#endif


                if (GetModuleFileNameEx(process_handle, nullptr, filename, MAX_PATH) <= 0)
                {
#ifdef DEBUG 
                    Logger::outputDebugString("GetModuleFileNameEx failed with error: " + String(GetLastError()));
#endif

                    CloseHandle(process_handle);
                    return {};
                }
            }
            CloseHandle(process_handle);

#ifdef DEBUG
            Logger::outputDebugString(filename);
#endif
            
            return filename;
        }
    }

    return {};
}

String WindowHelper::getActiveWindowPath()
{
    static HWND previousHandle = nullptr;
    static String savedPath;
    
    const auto handle = GetForegroundWindow();
    
    if (handle == nullptr)
        return {};
        
    if (handle == previousHandle && previousHandle != nullptr)
        return savedPath;
    
    previousHandle = handle;
    savedPath = getWindowProcessExePath(handle);
    return savedPath;
}

std::vector<WindowHelper::ProcessInfo> WindowHelper::getProcesses()
{
    std::vector<ProcessInfo> result;
        
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return result;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(snapshot, &pe32))
    {
        do
        {
            HWND hwnd = findMainWindow(pe32.th32ProcessID);
                
            if (!hwnd) continue;

            String path = getWindowProcessExePath(hwnd);

            const auto icon = detail::WindowingHelpers::createIconForFile(File(path));

            if (path.isEmpty())
            {
                wchar_t windowTitle[256];
                GetWindowTextW(hwnd, windowTitle, 256);
                OutputDebugStringW(L"Failed to get path for window: ");
                OutputDebugStringW(windowTitle);
                OutputDebugStringW(L"\n");
                
                // Try to get process path using pe32
                OutputDebugStringW(L"Process name from pe32: ");
                OutputDebugStringW(pe32.szExeFile);
                OutputDebugStringW(L"\n");

                continue;
            }
            
            result.emplace_back(path, icon);  
        }
        while (Process32NextW(snapshot, &pe32));
    }

    CloseHandle(snapshot);

    return result;
}

struct EnumData {
    DWORD process_id;
    HWND window;
};

static BOOL enumFunc (const HWND hwnd, const LPARAM lParam)
{
    auto& [pid, window] = *reinterpret_cast<EnumData*>(lParam);
    
    DWORD window_process_id = 0;
    GetWindowThreadProcessId(hwnd, &window_process_id);
        
    if (window_process_id == pid && isAppWindow(hwnd))
    {
        window = hwnd;
        return FALSE;
    }
        
    return TRUE;
}

HWND WindowHelper::findMainWindow(DWORD process_id)
{
    EnumData data = {.process_id = process_id, .window = nullptr};

    EnumWindows( enumFunc, reinterpret_cast<LPARAM>(&data));

    return data.window;
}


