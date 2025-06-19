#pragma once

#include "Controller.h"
#include "MappingListBoxModel.h"
#include "MappingTable.h"

class MainComponent final : public juce::Component
                            , public juce::Button::Listener
                            , public juce::Timer
{
public:
    explicit MainComponent(Controller& controller);
    ~MainComponent() override;
    
    void timerCallback() override;
    void buttonClicked(juce::Button*) override;
    void resized() override;

    void log(juce::String const& msg)
    {
        _log.insertTextAtCaret(msg);
    }

private:
    MappingListBoxModel _mappingListBoxModel;
    Controller&         _controller;
    
    juce::TextButton    _newMappingButton;
    juce::TextButton    _startStopButton;  
    juce::TextEditor    _log;
    MappingTable        _listBox;

    bool _stopping = false;
    
    JUCE_DECLARE_NON_MOVEABLE(MainComponent)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
