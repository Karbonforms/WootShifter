#pragma once
#include "Main.h"

class TrayIcon final : public juce::SystemTrayIconComponent
                     , public juce::ChangeListener
                     , public juce::ChangeBroadcaster
{
public:
    TrayIcon();
    
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;
    // void setMainWindow(juce::DocumentWindow* mainWindow);
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    juce::DocumentWindow* mainWindow;
    juce::PopupMenu menu;
};
