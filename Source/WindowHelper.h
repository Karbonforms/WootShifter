#pragma once
#include "Main.h"

#include <windows.h>
#include <vector>

class WindowHelper {
public:
    struct ProcessInfo {
        ProcessInfo(juce::String path, juce::Image icon)
            : path(std::move(path)),
              icon(std::move(icon))
        {
        }

        juce::String path;
        juce::Image icon;
    };

    static juce::String getWindowProcessExePath(HWND handle);
    static juce::String GetActiveWindowPath();
    static std::vector<ProcessInfo> GetProcesses();

private:
    // Helper function to find main window of a process
    static HWND findMainWindow(DWORD process_id);
};
