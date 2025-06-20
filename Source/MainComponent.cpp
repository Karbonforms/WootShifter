#include "MainComponent.h"

#include "Settings.h"
using namespace juce;

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
    }
}

void MainComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    const auto selectedItemIndex = comboBoxThatHasChanged->getSelectedItemIndex();
    Settings::getInstance()->saveWindowBehavior(static_cast<Settings::WindowBehavior>(selectedItemIndex));
    // WindowBehavior = selectedItemIndex;
}

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
    // _listBox.getHeader().setStretchToFitActive(true);
    // auto& header = _listBox.getHeader();
    // header.addColumn("Icon", static_cast<int>(ColumnIds::Icon), 30, 20, 40);
    // header.addColumn("Path", static_cast<int>(ColumnIds::Path), 120);
    // header.addColumn("Profile", static_cast<int>(ColumnIds::Profile), 90);
    // header.addColumn("Device", static_cast<int>(ColumnIds::Device), 90);
    // header.addColumn("Active", static_cast<int>(ColumnIds::Active), 40, 40);
    // header.addColumn("Delete", static_cast<int>(ColumnIds::Delete), 60, 60, 80);
    //
    // _listBox.getHeader().resizeAllColumnsToFit(_listBox.getWidth());

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
    // startTimer(400);
}

MainComponent::~MainComponent()
{
    _listBox.setModel(nullptr);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();

    _log.setBounds(bounds.removeFromBottom(80));
    _newMappingButton.setBounds(bounds.removeFromTop(30).withSize(100, 30));
    _windowBehavior.setBounds(bounds.removeFromBottom(30));
    _startStopButton.setBounds(bounds.removeFromBottom(30).withSizeKeepingCentre(100, 30));

    _listBox.setBounds(bounds);
}
