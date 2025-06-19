#include "MainComponent.h"
using namespace juce;

void MainComponent::timerCallback()
{
    const bool isThreadRunning = _controller.isThreadRunning();

    if (isThreadRunning && _stopping)
    {
        return;
    }

    if (isThreadRunning)
    {
        _startStopButton.setButtonText("Stop");
        _startStopButton.setColour(TextButton::buttonColourId, Colours::red);       
        return;
    }
    
    if (!isThreadRunning && _stopping)
    {
        _stopping = false;
        _startStopButton.setButtonText("Start");
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
        _controller.addMapping();
        _listBox.updateContent();
        return;
    }

    if (button == &_startStopButton)
    {
        if (_controller.isThreadRunning())
        {
            
            _stopping = true;
            _startStopButton.setButtonText("Stopping...");
            _controller.stop();
            startTimer(400);
        }
        else
        {
            _controller.start();
            _startStopButton.setButtonText("Stop");
            _startStopButton.setColour(TextButton::buttonColourId, Colours::red);
            log("Thread Started.\n");
        }
    }
}

MainComponent::MainComponent(Controller& controller)
: _mappingListBoxModel(_listBox), _controller(controller)
{
    setSize(600, 400);

    addAndMakeVisible(_newMappingButton);
    _newMappingButton.setButtonText("New");
    _newMappingButton.addListener(this);

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


    const bool isThreadRunning = controller.isThreadRunning();

    if (isThreadRunning)
    {
        _startStopButton.setButtonText("Stop");
        _startStopButton.setColour(TextButton::buttonColourId, Colours::red);
    }
    else
    {
        _startStopButton.setButtonText("Start");
        _startStopButton.setColour(TextButton::buttonColourId, Colours::green);       
    }
    
    _startStopButton.setButtonText(isThreadRunning ? "Stop" : "Start");
    _startStopButton.addListener(this);
    addAndMakeVisible(&_startStopButton);

    _mappingListBoxModel.setMappings(controller.GetMappings());
    controller.setLogout(&_log);

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
    _newMappingButton.setBounds(bounds.removeFromBottom(30));
    _startStopButton.setBounds(bounds.removeFromBottom(30));
    _listBox.setBounds(bounds);
}
