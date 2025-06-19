#pragma once


#include "MainComponent.h"

class Controller;

class MainWindow final   : public juce::DocumentWindow
                         , public juce::ChangeBroadcaster
                         , public juce::ChangeListener
{
    

public:
    MainWindow (const juce::String& name, Controller& controller);
    ~MainWindow() override = default;
    
    void closeButtonPressed() override
    {
        // This is called when the user tries to close this window. Here, we'll just
        // ask the app to quit when this happens, but you can change this to do
        // whatever you need.
        // getInstance()->systemRequestedQuit();
        setVisible(false);
        sendChangeMessage();
    }
    
    void changeListenerCallback(ChangeBroadcaster* source) override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    JUCE_DECLARE_NON_MOVEABLE (MainWindow)
};
