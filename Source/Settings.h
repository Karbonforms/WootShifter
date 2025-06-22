#pragma once
#include "Main.h"

class Settings final
{
public:
    Settings();
    ~Settings();

    enum class WindowBehavior : uint8_t
    {
        CloseToTray = 1,
        MinimizeToTray = 2,
        Normal = 3
    };
    
    enum class DetectionMethod : uint8_t
    {
        EventHook = 1,
        Polling = 2
    };

    void saveInterval(int interval) const;
    [[nodiscard]] int getInterval() const;
    
    void saveWindowBehavior(WindowBehavior wb) const;
    [[nodiscard]] WindowBehavior getWindowBehavior() const;

    void saveMethod ( DetectionMethod method ) const;
    [[nodiscard]] DetectionMethod getMethod () const;

    static juce::File getApplicationDataDirectory();
    static juce::File GetMappingsFile ();

    JUCE_DECLARE_SINGLETON(Settings, false)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Settings)
    JUCE_DECLARE_NON_MOVEABLE (Settings)
private:
    std::unique_ptr<juce::PropertiesFile> _properties;
};
