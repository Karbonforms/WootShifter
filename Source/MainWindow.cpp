#include "MainWindow.h"

#include "Settings.h"

using namespace juce;

MainWindow::MainWindow(const String& name)
: DocumentWindow( name
                , Desktop::getInstance().getDefaultLookAndFeel().findColour(backgroundColourId)
                , allButtons )
{
    setUsingNativeTitleBar(false);

    setContentOwned(new MainComponent(), true);

    setResizable(true, true);
    centreWithSize(getWidth(), getHeight());

    Component::setVisible(true);
}

void MainWindow::closeButtonPressed()
{
    // This is called when the user tries to close this window. Here, we'll just
    // ask the app to quit when this happens, but you can change this to do
    // whatever you need.

    const Settings::WindowBehavior wb = Settings::getInstance()->getWindowBehavior();

    if (wb == Settings::WindowBehavior::CloseToTray)
    {
        setVisible(false);
        sendChangeMessage();
    }
    else
    {
        JUCEApplication::getInstance()->systemRequestedQuit();
    }
}

void MainWindow::changeListenerCallback(ChangeBroadcaster* /*source*/)
{
    setVisible(true);
    toFront(true);
}

void MainWindow::minimisationStateChanged(bool isNowMinimised)
{
    const Settings::WindowBehavior wb = Settings::getInstance()->getWindowBehavior();

    if (wb == Settings::WindowBehavior::MinimizeToTray && isNowMinimised)
    {
        setVisible(false);
        sendChangeMessage();
    }

    DocumentWindow::minimisationStateChanged(isNowMinimised);
}
