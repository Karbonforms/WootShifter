
#include "MainComponent.h"
#include "Main.h"
#include "WootingDB.h"
#include <wooting-rgb-sdk.h>

#include "MainWindow.h"
#include "TrayIcon.h"

class WootShifterApplication final : public juce::JUCEApplication
{
public:
    WootShifterApplication() = default;

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return true; }


    void initialise (const String& /*commandLine*/) override
    {
        controller.start();
        mainWindow.reset (new MainWindow (getApplicationName(), controller));
        
        
        tray_icon.addChangeListener(mainWindow.get());
        mainWindow->addChangeListener(&tray_icon);
    }

    void shutdown() override
    {
        // Add your application's shutdown code here..

        mainWindow = nullptr; // (deletes our window)
    }

    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const String&) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

private:
    Controller controller;
    std::unique_ptr<MainWindow> mainWindow;
    TrayIcon tray_icon;
};

// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (WootShifterApplication)
