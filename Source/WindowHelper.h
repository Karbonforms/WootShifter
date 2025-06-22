#pragma once
#include "Main.h"
#include <windows.h>
#include <vector>

// #ifdef _WIN32
//     struct HWND__;
// typedef HWND__* HWND;
// #endif

class WindowHelper
{
public:
    struct ProcessInfo
    {
        ProcessInfo(juce::String path, juce::Image icon)
        : Path(std::move(path))
        , Icon(std::move(icon))
        {}

        juce::String Path;
        juce::Image Icon;
    };
    
    static juce::String             GetActiveWindowPath();
    static std::vector<ProcessInfo> GetProcesses();
    static juce::String             GetWindowProcessExePath(HWND handle);

    static bool                     RegisterWindowEvents ();
    static void                     UnRegisterWindowEvents ();
    static bool                     IsEventHookValid();

private:
    // static HWINEVENTHOOK _eventHook;
    // static HWND findMainWindow(DWORD processId);
    // static void WinEventProc ( HWINEVENTHOOK hook
    //                          , DWORD event
    //                          , HWND hwnd
    //                          , LONG idObject
    //                          , LONG idChild
    //                          , DWORD idEventThread
    //                          , DWORD dwmsEventTime );
};
