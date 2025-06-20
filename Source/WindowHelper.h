#pragma once
#include "Main.h"

#include <vector>
#include <windows.h>

class WindowHelper
{
public:
    struct ProcessInfo
    {
        ProcessInfo(juce::String path, juce::Image icon)
        : Path(std::move(path))
        , Icon(std::move(icon))
        {
        }

        juce::String Path;
        juce::Image Icon;
    };
    
    static juce::String getActiveWindowPath();
    static std::vector<ProcessInfo> getProcesses();

private:
    // Helper function to find main window of a process
    static HWND findMainWindow(DWORD process_id);
    static juce::String getWindowProcessExePath(HWND handle);
};
