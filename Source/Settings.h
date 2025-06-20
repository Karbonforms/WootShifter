#pragma once
#include "Main.h"

class Settings final
{
public:
    Settings();
    ~Settings();

    enum class WindowBehavior
    {
        CloseToTray = 0,
        MinimizeToTray = 1,
        Normal = 2
    };

    void saveInterval(int interval) const;
    [[nodiscard]] int getInterval() const;
    
    void saveWindowBehavior(WindowBehavior wb) const;
    [[nodiscard]] WindowBehavior getWindowBehavior() const;

    static juce::File getApplicationDataDirectory();

    JUCE_DECLARE_SINGLETON(Settings, false)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Settings)
    JUCE_DECLARE_NON_MOVEABLE (Settings)
private:
    std::unique_ptr<juce::PropertiesFile> _properties;
};
