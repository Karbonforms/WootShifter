#include "WindowHelper.h"
#include <psapi.h>
#include <tlhelp32.h>
#include "juce_gui_basics/detail/juce_WindowingHelpers.h"

using namespace juce;

using detail::WindowingHelpers;

namespace
{
    struct EnumData {
        DWORD process_id;
        HWND window;
    };
    
    bool IsAppWindow(const HWND hwnd)
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

        const LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
        const LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

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

    BOOL EnumFunc (const HWND hwnd, const LPARAM lParam)
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
}

String WindowHelper::getWindowProcessExePath(const HWND handle)
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
#ifdef DEBUG
            Logger::outputDebugString("OpenProcess failed with error: " + String(GetLastError()));
#endif
 
            // Try with fewer access rights
            processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
            if (!processHandle)
            {
#ifdef DEBUG
                Logger::outputDebugString("OpenProcess (limited) failed with error: " + String(GetLastError()));
#endif
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
#ifdef DEBUG 
                Logger::outputDebugString("QueryFullProcessImageNameA failed with error: " + String(GetLastError()));
#endif


                if (GetModuleFileNameEx(processHandle, nullptr, filename, MAX_PATH) <= 0)
                {
#ifdef DEBUG 
                    Logger::outputDebugString("GetModuleFileNameEx failed with error: " + String(GetLastError()));
#endif

                    CloseHandle(processHandle);
                    return {};
                }
            }
            CloseHandle(processHandle);

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
    
    if (handle == nullptr) return {};
        
    if (handle == previousHandle && previousHandle != nullptr) return savedPath;
    
    previousHandle = handle;
    savedPath = getWindowProcessExePath(handle);
    return savedPath;
}

std::vector<WindowHelper::ProcessInfo> WindowHelper::getProcesses()
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

            String path = getWindowProcessExePath(hwnd);
            
            if (path.isEmpty()) continue;
            
            const auto icon = WindowingHelpers::createIconForFile(File(path));
            
            result.emplace_back(path, icon);  
        }
        while (Process32NextW(snapshot, &pe32));
    }

    CloseHandle(snapshot);

    return result;
}

HWND WindowHelper::findMainWindow(DWORD process_id)
{
    EnumData data = {.process_id = process_id, .window = nullptr};

    EnumWindows( EnumFunc, reinterpret_cast<LPARAM>(&data));

    return data.window;
}


