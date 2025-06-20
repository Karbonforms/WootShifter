#pragma once

#include "Controller.h"
#include "MappingListBoxModel.h"
#include "MappingTable.h"

class MainComponent final : public juce::Component
                          , public juce::Button::Listener
                          , public juce::ComboBox::Listener
                          , public juce::Timer
{
public:
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    explicit MainComponent();
    ~MainComponent() override;
    
    void timerCallback() override;
    void buttonClicked(juce::Button*) override;
    void resized() override;

    void log(juce::String const& msg)
    {
        _log.insertTextAtCaret(msg);
    }

    int WindowBehavior = 0;

private:
    MappingListBoxModel _mappingListBoxModel;
    // Controller&         _controller;
    
    juce::TextButton    _newMappingButton;
    juce::TextButton    _startStopButton;  
    juce::TextEditor    _log;
    juce::TooltipWindow _tooltip;
    juce::ComboBox      _windowBehavior;
    
    MappingTable        _listBox;

    bool _stopping = false;
    
    JUCE_DECLARE_NON_MOVEABLE(MainComponent)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
