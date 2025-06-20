#include "MainComponent.h"

#include "Settings.h"
using namespace juce;

MainComponent::MainComponent()
: _mappingListBoxModel(_listBox)
{
    setSize(600, 400);

    addAndMakeVisible(_newMappingButton);
    _newMappingButton.setButtonText("New");
    _newMappingButton.addListener(this);
    _newMappingButton.setTooltip("Add a new Mapping");
    _newMappingButton.setColour(TextButton::buttonColourId, Colours::green);

    addAndMakeVisible(&_listBox);
    _listBox.setModel(&_mappingListBoxModel);

    _log.setReadOnly(true);
    _log.setMultiLine(true);
    _log.setScrollbarsShown(true);
    addAndMakeVisible(&_log);

    const auto controller = Controller::getInstance();

    const bool isThreadRunning = controller->isThreadRunning();

    if (isThreadRunning)
    {
        _startStopButton.setButtonText("Stop Profile Switching");
        _startStopButton.setColour(TextButton::buttonColourId, Colours::red);
    }
    else
    {
        _startStopButton.setButtonText("Start Profile Switching");
        _startStopButton.setColour(TextButton::buttonColourId, Colours::green);
    }

    _startStopButton.addListener(this);
    _startStopButton.setTooltip("Start/Stop the automatic Profile Switching");
    addAndMakeVisible(&_startStopButton);

    _mappingListBoxModel.setMappings(controller->getMappings());
    controller->setLogout(&_log);

    _windowBehavior.addItem("[x] Closes to SysTray", 1);
    _windowBehavior.addItem("[_] Minimises to SysTray", 2);
    _windowBehavior.addItem("Closes and Minimises normally", 3);
    _windowBehavior.setSelectedItemIndex(static_cast<int>(Settings::getInstance()->getWindowBehavior())
    );
    _windowBehavior.addListener(this);
    addAndMakeVisible(_windowBehavior);

    _tooltip.setMillisecondsBeforeTipAppears(700);
    addChildComponent(_tooltip);

    
    _interval.setJustification(Justification::centred);
    _interval.setTooltip("Interval in ms");
    _interval.setInputRestrictions(4, "0123456789");
    _interval.setText(String(Settings::getInstance()->getInterval()), dontSendNotification);
    // _interval.addListener(this);
    addAndMakeVisible(_interval);

    _intervalLabel.setText("Interval (ms):", dontSendNotification);
    addAndMakeVisible(_intervalLabel);

    _intervalButton.setButtonText("Apply");
    _intervalButton.addListener(this);
    addAndMakeVisible(_intervalButton);
}

MainComponent::~MainComponent()
{
    _listBox.setModel(nullptr);
}

void MainComponent::timerCallback()
{
    const bool isThreadRunning = Controller::getInstance()->isThreadRunning();

    if (isThreadRunning && _stopping)
    {
        return;
    }

    if (isThreadRunning)
    {
        _startStopButton.setButtonText("Stop Profile Switching");
        _startStopButton.setColour(TextButton::buttonColourId, Colours::red);
        return;
    }

    if (!isThreadRunning && _stopping)
    {
        _stopping = false;
        _startStopButton.setButtonText("Start Profile Switching");
        _startStopButton.setColour(TextButton::buttonColourId, Colours::green);
        stopTimer();
        log("Thread Stopped.\n");
        return;
    }
}

void MainComponent::buttonClicked(Button* button)
{
    if (button == &_newMappingButton)
    {
        Controller::getInstance()->addMapping();
        _listBox.updateContent();
        return;
    }

    if (button == &_startStopButton)
    {
        const auto controller = Controller::getInstance();
        if (controller->isThreadRunning())
        {
            _stopping = true;
            _startStopButton.setButtonText("Stopping...");
            Controller::getInstance()->stop();
            startTimer(400);
        }
        else
        {
            controller->start();
            _startStopButton.setButtonText("Stop");
            _startStopButton.setColour(TextButton::buttonColourId, Colours::red);
            log("Thread Started.\n");
        }
        return;
    }

    if (button == &_intervalButton)
    {
        auto t = _interval.getText().trim();
    
        if (t.isEmpty())
        {
            _interval.setText(String(Settings::getInstance()->getInterval()), dontSendNotification);
            return;
        }

        const int newInterval = t.getIntValue();

        if (newInterval >= 500)
        {
            Settings::getInstance()->saveInterval(newInterval);
        }
        else
        {
            _interval.setText(String(Settings::getInstance()->getInterval()), dontSendNotification);
        }
    }
}

void MainComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    const auto selectedItemIndex = comboBoxThatHasChanged->getSelectedItemIndex();
    Settings::getInstance()->saveWindowBehavior(static_cast<Settings::WindowBehavior>(selectedItemIndex));
    // WindowBehavior = selectedItemIndex;
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();

    _log.setBounds(bounds.removeFromBottom(80));

    auto removeFromTop = bounds.removeFromTop(30);
    _newMappingButton.setBounds(removeFromTop.removeFromLeft(100));
    _startStopButton.setBounds(removeFromTop.removeFromRight(100));
    
    _windowBehavior.setBounds(bounds.removeFromBottom(30));

    auto intervalBounds = bounds.removeFromBottom(30);
    _intervalLabel.setBounds(intervalBounds.removeFromLeft(100));
    _interval.setBounds(intervalBounds.removeFromLeft(100));
    _intervalButton.setBounds(intervalBounds.removeFromLeft(100));
    _interval.setFont(FontOptions(static_cast<float>(_interval.getHeight())));
    
    _listBox.setBounds(bounds);
}
