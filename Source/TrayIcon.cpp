#include "TrayIcon.h"

#include "Controller.h"

using namespace juce;

const char* startText = "Start";
const char* stopText = "Stop";

TrayIcon::TrayIcon ()
{
    auto controller = Controller::getInstance();
    controller->addActionListener(this);
    const auto image = ImageFileFormat::loadFrom(BinaryData::small_png, BinaryData::small_pngSize);
    setIconImage(image, image);
    setIconTooltip(JUCEApplication::getInstance()->getApplicationName());
}

void TrayIcon::mouseEnter ( const MouseEvent& event )
{
    SystemTrayIconComponent::mouseEnter(event);
}

void TrayIcon::mouseDown ( const MouseEvent& event )
{
    if (event.mods.isRightButtonDown())
    {
        const auto controller = Controller::getInstance();
        _menu.clear();
        _menu.addItem(1, controller->isRunning() ? stopText : startText);
        _menu.addItem(2, "Show Window");
        _menu.addItem(3, "Exit");
        
        _menu.showMenuAsync(PopupMenu::Options()
                            , [this, controller]( const int result )
                                {
                                    if (result == 1)
                                    {
                                        if (controller->isRunning())
                                        {
                                            controller->stop();
                                        }
                                        else
                                        {
                                            controller->start();   
                                        }
                                    }
                                    else if (result == 2)
                                    {
                                        sendChangeMessage();
                                    }
                                    else if (result == 3)
                                    {
                                        // Actually quit the application
                                        JUCEApplication::getInstance()->systemRequestedQuit();
                                    }
                                });
    }
}

void TrayIcon::mouseDoubleClick ( const MouseEvent& )
{
    sendChangeMessage();
}

void TrayIcon::changeListenerCallback ( ChangeBroadcaster* /*source*/ )
{
    const String msg = JUCEApplication::getInstance()->getApplicationName();
    showInfoBubble(msg, "Running in Notification Area!");
}

void TrayIcon::actionListenerCallback ( const String& message )
{
    String s = JUCEApplication::getInstance()->getApplicationName();
    s << " " << message << "\n\nRight click for options.";
    setIconTooltip(s);
}
