#include "Settings.h"

using namespace juce;

JUCE_IMPLEMENT_SINGLETON(Settings)

File Settings::getApplicationDataDirectory()
{
    const auto appdata = File::getSpecialLocation(File::windowsLocalAppData);
    const auto appDataDir = appdata.getChildFile(JUCEApplication::getInstance()->getApplicationName());

    if (!appDataDir.exists())
    {
        auto r = appDataDir.createDirectory();
        if (r.failed())
        {
            Logger::outputDebugString("Failed to create directory: " + appDataDir.getFullPathName());
        }
    }

    return appDataDir;
}

Settings::Settings()
{
    PropertiesFile::Options options;
    options.applicationName = "WootShifter";
    options.filenameSuffix = ".settings";
    options.folderName = "WootShifter";
    options.osxLibrarySubFolder = "Application Support";
    options.storageFormat = PropertiesFile::storeAsXML;

    const auto appDataDir = getApplicationDataDirectory();
    
    auto settingsFile = appDataDir.getChildFile(options.applicationName + options.filenameSuffix);
    _properties = std::make_unique<PropertiesFile>(settingsFile, options);
}

Settings::~Settings()
{
    _properties->saveIfNeeded();
    _properties = nullptr;
}

void Settings::saveInterval ( int interval ) const
{
    _properties->setValue("interval", interval);
    _properties->saveIfNeeded();
}

int Settings::getInterval () const
{
    return _properties->getIntValue("interval", 1000);
}

void Settings::saveWindowBehavior(WindowBehavior wb)  const
{
    _properties->setValue("window_behavior", static_cast<int>(wb));
    _properties->saveIfNeeded();
}

Settings::WindowBehavior Settings::getWindowBehavior() const
{
    return static_cast<WindowBehavior>(_properties->getIntValue("window_behavior", 0));
}
