#pragma once
#include "Main.h"

class TrayIcon final : public juce::SystemTrayIconComponent
                     , public juce::ChangeListener
                     , public juce::ChangeBroadcaster
                     , public juce::ActionListener
{
public:
    TrayIcon();
    
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;
    
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void actionListenerCallback ( const juce::String& message ) override;

private:
    juce::PopupMenu _menu;
};
