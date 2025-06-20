#pragma once

#include "Main.h"

class Controller final : public juce::Thread
                       , public juce::ActionBroadcaster
{
public:
    Controller();
    ~Controller() override;
    
    void setLogout(juce::TextEditor* editor);

    void saveMappings() const;
    // uint8_t find_profile_index(String profile_name);

    void log(juce::String const& msg) const;
    
    Mappings* getMappings() { return &_mappings; }
    
    void run() override;
    void addMapping(bool create_default_mapping = false);
    
    void stop();
    void start();

    static void assignProfilesToDevices(const ProfilesByDevice& profilesByDevice);
    
    JUCE_DECLARE_SINGLETON(Controller, true)
private:
    // ProfilesByDevice profilesByDevice;
    Mappings            _mappings;
    juce::String        _currentDeviceId;
    int                 _interval = 1000;
    juce::TextEditor*   _logout = nullptr;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Controller)
    JUCE_DECLARE_NON_MOVEABLE(Controller)
};
