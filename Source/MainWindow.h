#pragma once

#include "MainComponent.h"

class MainWindow final   : public juce::DocumentWindow
                         , public juce::ChangeBroadcaster
                         , public juce::ChangeListener
{
public:
    MainWindow (const juce::String& name);
    ~MainWindow() override = default;
    
    void closeButtonPressed() override;
    void changeListenerCallback(ChangeBroadcaster* source) override;
    void minimisationStateChanged(bool isNowMinimised) override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    JUCE_DECLARE_NON_MOVEABLE (MainWindow)
};
