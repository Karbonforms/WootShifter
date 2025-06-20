#pragma once


#include "MainComponent.h"

class Controller;

class MainWindow final   : public juce::DocumentWindow
                         , public juce::ChangeBroadcaster
                         , public juce::ChangeListener
{
    MainComponent* maincomp {nullptr};

public:
    MainWindow (const juce::String& name);
    ~MainWindow() override = default;
    
    void closeButtonPressed() override;

    void changeListenerCallback(ChangeBroadcaster* source) override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    JUCE_DECLARE_NON_MOVEABLE (MainWindow)

public:
    void minimisationStateChanged(bool isNowMinimised) override;
};
