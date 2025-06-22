#include "Main.h"
#include "MainComponent.h"
#include "MainWindow.h"
#include "Settings.h"
#include "TrayIcon.h"

#include "version.h"

class WootShifterApplication final : public juce::JUCEApplication
{
public:
    WootShifterApplication() = default;

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return false; }
    
    void initialise (const String& /*commandLine*/) override
    {
        Controller::getInstance()->start();
        
        const auto windowTitle = getApplicationName() + " - " + VERSION_STRING;
        _mainWindow.reset (new MainWindow (windowTitle));
        
        _trayIcon.addChangeListener(_mainWindow.get());
        _mainWindow->addChangeListener(&_trayIcon);
    }

    void shutdown() override
    {
        Controller::getInstance()->stop();
        Controller::deleteInstance();
        Settings::deleteInstance();
        _mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const String&) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

private:
    std::unique_ptr<MainWindow> _mainWindow;
    TrayIcon _trayIcon;
};

// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (WootShifterApplication)
