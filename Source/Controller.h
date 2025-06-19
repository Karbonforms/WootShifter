#pragma once

#include "Main.h"

class Controller final : public juce::Thread
{
private:
    // ProfilesByDevice profilesByDevice;
    Mappings mappings;
    juce::String  currentDeviceID;
    int interval = 1000;
    juce::TextEditor* logout = nullptr;
public:
    static void assignProfilesToDevices(const ProfilesByDevice& profilesByDevice);
    Controller();

    void setLogout(juce::TextEditor* editor);

    void saveMappings() const;
    // uint8_t find_profile_index(String profile_name);

    void log(juce::String const& msg) const;

    ~Controller() override
    {
        stopThread(interval);
        saveMappings();
    }

    Mappings* GetMappings() { return &mappings; }

    void run() override;
    void addMapping(bool create_default_mapping = false);
    
    void stop() { stopThread(interval); }
    void start() { startThread(Priority::low); }

    JUCE_DECLARE_NON_COPYABLE(Controller)
    JUCE_DECLARE_NON_MOVEABLE(Controller)
};
