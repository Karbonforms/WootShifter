#include "TrayIcon.h"

using namespace juce;

TrayIcon::TrayIcon()
{
    // Image image = ImageFileFormat::loadFrom(File(
    //     R"(C:\Program Files\Applied Acoustics Systems\String Studio VS-3\Bitmaps\artwork_letter_g@2x.png)"));
    //
    // JUCEApplication::getInstance().get
    const auto image = ImageFileFormat::loadFrom (BinaryData::small_png, BinaryData::small_pngSize);
    setIconImage (image, image);
    setIconTooltip (JUCEApplication::getInstance()->getApplicationName());
    
    menu.addItem(1, "Show Window");
    menu.addItem(2, "Exit");
}

void TrayIcon::mouseEnter(const MouseEvent& event)
{
    SystemTrayIconComponent::mouseEnter(event);
}

void TrayIcon::mouseDown(const MouseEvent& event)
{
    if (event.mods.isRightButtonDown())
    {
        menu.showMenuAsync(PopupMenu::Options()
                            ,[this](int result)
        {
            if (result == 1 /*&& mainWindow != nullptr*/)
            {
                // mainWindow->setVisible(true);
                // mainWindow->toFront(true);
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

void TrayIcon::mouseDoubleClick(const MouseEvent&)
{
    // if (mainWindow != nullptr)
    // {
    //     mainWindow->setVisible(true);
    //     mainWindow->toFront(true);
    // }
    sendChangeMessage();
}

// void TrayIcon::setMainWindow(DocumentWindow* main)
// {
//     mainWindow = main;
// }

void TrayIcon::changeListenerCallback(juce::ChangeBroadcaster*)
{
    String msg = JUCEApplication::getInstance()->getApplicationName();
    showInfoBubble(msg,"Running in Notification Area!");
}
