#include "MainWindow.h"

MainWindow::MainWindow(const juce::String& name, Controller& controller): DocumentWindow (name,
    juce::Desktop::getInstance().getDefaultLookAndFeel()
                                .findColour (backgroundColourId),
    allButtons)
{
    setUsingNativeTitleBar (true);
    setContentOwned (new MainComponent(controller), true);

    setResizable (true, true);
    centreWithSize (getWidth(), getHeight());

    Component::setVisible (true);
}

void MainWindow::changeListenerCallback(ChangeBroadcaster* /*source*/)
{
    setVisible(true);
    toFront(true);
}
