#include "MainComponent.h"

#include "Settings.h"
using namespace juce;

MainComponent::MainComponent()
// : _mappingListBoxModel(&_listBox)
{
    _mappingListBoxModel = std::make_unique<MappingListBoxModel>(&_listBox);
    
    setSize(600, 400);

    addAndMakeVisible(_newMappingButton);
    _newMappingButton.setButtonText("New");
    _newMappingButton.addListener(this);
    _newMappingButton.setTooltip("Add a new Mapping");
    _newMappingButton.setColour(TextButton::buttonColourId, Colours::green);

    addAndMakeVisible(&_listBox);
    _listBox.setModel(_mappingListBoxModel.get());

    _log.setReadOnly(true);
    _log.setMultiLine(true);
    _log.setScrollbarsShown(true);
    addAndMakeVisible(&_log);

    const auto controller = Controller::getInstance();

    controller->addActionListener(this);

    if (controller->isRunning())
    {
        _startStopButton.setButtonText(stopText);
        _startStopButton.setColour(TextButton::buttonColourId, Colours::red);
    }
    else
    {
        _startStopButton.setButtonText(startText);
        _startStopButton.setColour(TextButton::buttonColourId, Colours::green);
    }

    _startStopButton.addListener(this);
    _startStopButton.setTooltip("Start/Stop the automatic Profile Switching");
    addAndMakeVisible(&_startStopButton);

    _mappingListBoxModel->setMappings(controller->getMappings());
    controller->setLogout(&_log);

    _windowBehavior.addItem("[x] Closes to SysTray", static_cast<int>(Settings::WindowBehavior::CloseToTray));
    _windowBehavior.addItem("[_] Minimises to SysTray", static_cast<int>(Settings::WindowBehavior::MinimizeToTray));
    _windowBehavior.addItem("Closes and Minimises normally", static_cast<int>(Settings::WindowBehavior::Normal));
    _windowBehavior.setSelectedId(static_cast<int>(Settings::getInstance()->getWindowBehavior()));
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
    
    _methodCombo.addItem("Detection Method: Windows Event Hook", static_cast<int>(Settings::DetectionMethod::EventHook));
    _methodCombo.addItem("Detection Method: Polling", static_cast<int>(Settings::DetectionMethod::Polling));

    auto detectionMethod = Settings::getInstance()->getMethod();

    if (detectionMethod == Settings::DetectionMethod::EventHook)
    {
        _interval.setVisible(false);
        _intervalLabel.setVisible(false);
        _intervalButton.setVisible(false);
    }
    
    _methodCombo.setSelectedId(static_cast<int>(detectionMethod));
    _methodCombo.addListener(this);
    addAndMakeVisible(_methodCombo);
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
        _startStopButton.setButtonText(stopText);
        _startStopButton.setColour(TextButton::buttonColourId, Colours::red);
        return;
    }

    if (!isThreadRunning && _stopping)
    {
        _stopping = false;
        _startStopButton.setButtonText(startText);
        _startStopButton.setColour(TextButton::buttonColourId, Colours::green);
        stopTimer();
        log("Polling Thread Stopped.\n");
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
        if (controller->isRunning())
        {
            controller->stop();
            if (Settings::getInstance()->getMethod() == Settings::DetectionMethod::Polling)
            {
                _startStopButton.setButtonText("Stopping...");
                _stopping = true;
                startTimer(400);
            }
            else
            {
                _startStopButton.setButtonText(startText);
                _startStopButton.setColour(TextButton::buttonColourId, Colours::green);
            }
            
        }
        else
        {
            controller->start();
            _startStopButton.setButtonText(stopText);
            _startStopButton.setColour(TextButton::buttonColourId, Colours::red);
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

void MainComponent::updateStartStopButton (bool stopped)
{
    if (stopped)
    {
        _startStopButton.setButtonText(startText);
        _startStopButton.setColour(TextButton::buttonColourId, Colours::green);
        return;   
    }
    else
    {
        _startStopButton.setButtonText(stopText);
        _startStopButton.setColour(TextButton::buttonColourId, Colours::red);
    }
}

void MainComponent::actionListenerCallback ( const String& message )
{
    updateStartStopButton(message == Controller::StopMsg);
}

void MainComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &_windowBehavior)
    {
        const auto item = comboBoxThatHasChanged->getSelectedId();
        Settings::getInstance()->saveWindowBehavior(static_cast<Settings::WindowBehavior>(item));
        return;
    }
    
    if (comboBoxThatHasChanged == &_methodCombo)
    {
        const auto method = static_cast<Settings::DetectionMethod>(comboBoxThatHasChanged->getSelectedId());

        if (method != Settings::getInstance()->getMethod())
        {
            const auto controller = Controller::getInstance();
            
            if (controller->isRunning())
            {
                controller->stop();
                Settings::getInstance()->saveMethod((method));
                controller->start();
            }
            else
            {
                Settings::getInstance()->saveMethod((method));
            }

            if (method == Settings::DetectionMethod::Polling)
            {
                _interval.setVisible(true);
                _intervalLabel.setVisible(true);
                _intervalButton.setVisible(true);
                log("Polling @" + String(Settings::getInstance()->getInterval()) + "ms\n");
            }
            else
            {
                _interval.setVisible(false);
                _intervalLabel.setVisible(false);
                _intervalButton.setVisible(false);
                log("Event Hook Activated\n");           
            }
        }
    }
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();

    _log.setBounds(bounds.removeFromBottom(80));

    auto removeFromTop = bounds.removeFromTop(30);
    _newMappingButton.setBounds(removeFromTop.removeFromLeft(100));
    _startStopButton.setBounds(removeFromTop.removeFromRight(100));
    
    _windowBehavior.setBounds(bounds.removeFromBottom(30));

    _methodCombo.setBounds(bounds.removeFromBottom(30));

    auto intervalBounds = bounds.removeFromBottom(30);
    _intervalLabel.setBounds(intervalBounds.removeFromLeft(100));
    _interval.setBounds(intervalBounds.removeFromLeft(100));
    _intervalButton.setBounds(intervalBounds.removeFromLeft(100));
    _interval.setFont(FontOptions(static_cast<float>(_interval.getHeight())));
    
    _listBox.setBounds(bounds);
}
