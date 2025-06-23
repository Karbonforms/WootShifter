#pragma once

#include "Controller.h"
#include "MappingListBoxModel.h"
#include "MappingTable.h"

class MainComponent final : public juce::Component
                          , public juce::Button::Listener
                          , public juce::ComboBox::Listener
                          // , public juce::TextEditor::Listener
                          , public juce::Timer
{
public:
    // void textEditorTextChanged ( juce::TextEditor& ) override;

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
    
    // Controller&         _controller;
    
    juce::TextButton    _newMappingButton;
    juce::TextButton    _startStopButton;  
    juce::TextEditor    _log;
    juce::TooltipWindow _tooltip;
    juce::ComboBox      _windowBehavior;
    juce::Label         _intervalLabel;
    juce::TextEditor    _interval;
    juce::TextButton    _intervalButton;
    juce::Label         _methodLabel;
    juce::ComboBox      _methodCombo;
    
    MappingTable        _listBox;
    std::unique_ptr<MappingListBoxModel> _mappingListBoxModel;
    
    bool _stopping = false;
    const char* stopText = "Stop Profile Switching";
    const char* startText = "Start Profile Switching";

    JUCE_DECLARE_NON_MOVEABLE(MainComponent)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
