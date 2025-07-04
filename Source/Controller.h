﻿#pragma once
#include "Main.h"
#include "Mapping.h"
#include "WindowHelper.h"

class Controller final : public juce::Thread
                       , public juce::ActionBroadcaster
{
public:
    Controller();
    ~Controller() override;
    
    void handleProfileActivation ( juce::String path );
    void handleProfileActivation ( HWND hwnd );
    
    void setLogout(juce::TextEditor* editor);
    void log(juce::String const& msg) const;
    
    void saveMappings() const;
    Mappings* getMappings() { return &_mappings; }
    void addMapping(bool create_default_mapping = false);

    static String StartMsg; // "started";
    static String StopMsg; // = "stopped";
    
    void stop();
    void start();
    
    void run() override;
    bool isRunning () const
    {
        return isThreadRunning() || WindowHelper::IsEventHookValid();
    }

    String debugMappings() const
    {
        String s ("Mappings: ");
        s << _mappings.size() << juce::newLine;
        for (auto& mapping : _mappings)
        {
            s << mapping->toString();
        }
        return s;
    }

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
