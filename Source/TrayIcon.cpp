#include "TrayIcon.h"

#include "Controller.h"

using namespace juce;

TrayIcon::TrayIcon ()
{
    Controller::getInstance()->addActionListener(this);
    const auto image = ImageFileFormat::loadFrom(BinaryData::small_png, BinaryData::small_pngSize);
    setIconImage(image, image);
    setIconTooltip(JUCEApplication::getInstance()->getApplicationName());


    _menu.addItem(1, "Show Window");
    _menu.addItem(2, "Exit");
}

void TrayIcon::mouseEnter ( const MouseEvent& event )
{
    SystemTrayIconComponent::mouseEnter(event);
}

void TrayIcon::mouseDown ( const MouseEvent& event )
{
    if (event.mods.isRightButtonDown())
    {
        _menu.showMenuAsync(PopupMenu::Options()
                            , [this]( const int result )
                                {
                                    if (result == 1)
                                    {
                                        sendChangeMessage();
                                    }
                                    else if (result == 2)
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
