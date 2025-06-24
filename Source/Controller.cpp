#include "Controller.h"
#include "Mapping.h"
#include "Settings.h"
#include "WootingControl.h"
#include "WootingDB.h"

#include <windows.h>

#define JSON_NOEXCEPTION 1
#include "json.hpp"

using namespace juce;
using namespace nlohmann;

String Controller::StopMsg = "stopped";
String Controller::StartMsg = "started";

JUCE_IMPLEMENT_SINGLETON(Controller);

Controller::Controller ()
: Thread("Controller")
{
    Mapping::devices(WootingControl::RetrieveConnectedDevices());

    _currentDeviceId = Mapping::devices().back().DeviceId();

    for (auto& device : Mapping::devices())
    {
        DBG(device.ModelName);
    }

    const auto profilesByDevice = WootingDB::retrieveProfileData();

    if (profilesByDevice.empty())
    {
        JUCEApplication::quit();
    }

    assignProfilesToDevices(profilesByDevice);

    // const auto appdir = Settings::getApplicationDataDirectory();

    const auto mappingsFile = Settings::GetMappingsFile();

    if (mappingsFile.existsAsFile())
    {
        auto jsonString = mappingsFile.loadFileAsString().toStdString();

        if (!json::accept(jsonString))
        {
            DBG("Invalid JSON");
            AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Invalid JSON", "Invalid JSON in mappings.json");
            JUCEApplication::quit();
            return;
        }

        json jsonMappings = json::parse(jsonString);

        for (const auto& jsonMapping : jsonMappings)
        {
            auto mappingPtr = std::make_unique<Mapping>();

            mappingPtr->deviceId(std::string(jsonMapping["DeviceId"]));
            mappingPtr->path(std::string(jsonMapping["Path"]));
            mappingPtr->profileName(std::string(jsonMapping["Profile"]));
            mappingPtr->isActive(jsonMapping["IsActive"]);

            _mappings.emplace_back(std::move(mappingPtr));
        }
    }

    if (_mappings.empty())
        addMapping(true);

    std::ranges::sort(_mappings, []( const auto& a, const auto& b )
    {
        if (a->isDefault() != b->isDefault())
        {
            return a->isDefault(); // true values will come first
        }
        return false; // don't change the relative order of other items
    });

    for (const auto& mapping : _mappings)
    {
        mapping->initialize();
    }
}

void Controller::assignProfilesToDevices ( const ProfilesByDevice& profilesByDevice )
{
    if (!Mapping::devices().empty())
    {
        for (Device& device : Mapping::devices())
        {
            auto deviceId = device.DeviceId();

            auto it = profilesByDevice.find(device.DeviceId());
            if (it != profilesByDevice.end())
            {
                device.ProfileInfos = it->second;
            }
        }
    }
}

void Controller::setLogout ( TextEditor* editor )
{
    this->_logout = editor;

    if (_logout)
    {
        String data;

        data << "Devices and Profiles found:\n\n";
        for (auto& device : Mapping::devices())
        {
            data << device.DeviceId() << " " << device.ModelName << "\n\n";
            for (auto const& profile : device.ProfileInfos)
            {
                data << "\t" << profile.Name << " " << profile.Uid << "\n";
            }
        }
        
        log(data);
    }
}

void Controller::saveMappings () const
{
    if (_mappings.empty())
        return;

    json jsonMappings = json::array();

    for (const auto& mapping : _mappings)
    {
        json jsonMapping;
        jsonMapping["DeviceId"] = mapping->deviceId().toStdString();
        jsonMapping["Path"] = mapping->path().toStdString();
        jsonMapping["Profile"] = mapping->profileName().toStdString();
        jsonMapping["IsActive"] = mapping->isActive();

        jsonMappings.push_back(jsonMapping);
    }

    const File mappingsFile = Settings::GetMappingsFile();

    if (mappingsFile.replaceWithText(jsonMappings.dump(4)))
    {
        DBG("Saved mappings");
    }
}

void Controller::log ( String const& msg ) const
{
    if (_logout)
    {
        _logout->insertTextAtCaret(msg);
        _logout->insertTextAtCaret("\n");
    }
}

Controller::~Controller ()
{
    stopThread(_interval);
    saveMappings();
}

void Controller::handleProfileActivation ( String path )
{
    auto pred = [&path]( const std::unique_ptr<Mapping>& m )
    {
        return m->path() == path && m->isActive();
    };

    const auto mappingIterator = std::ranges::find_if(_mappings, pred);

    if (mappingIterator != _mappings.end())
    {
        const auto profileIndex = mappingIterator->get()->profileIndex();
        const auto deviceIndex = mappingIterator->get()->device().Index;

        if (WootingControl::setActiveProfileIndex(deviceIndex, profileIndex))
        {
            const auto profile_name = (*mappingIterator)->profileName();
            const MessageManagerLock lock;
            log("Active profile: " + profile_name);
        }
    }
    else
    {
        for (const auto& mapping : _mappings)
        {
            if (mapping->isDefault())
            {
                const auto profileIndex = mapping->profileIndex();
                const auto deviceIndex = mapping->device().Index;
                
                if (WootingControl::setActiveProfileIndex(deviceIndex, profileIndex))
                {
                    const MessageManagerLock lock;
                    log("Active profile: " + mapping->profileName());
                }

                break;
            }
        }
    }
}

void Controller::handleProfileActivation ( const HWND hwnd )
{
    static HWND previousHandle = nullptr;
    static String savedPath;
    
    if (hwnd == nullptr) return;

    if (hwnd == previousHandle && previousHandle != nullptr) return;

    previousHandle = hwnd;
    savedPath = WindowHelper::GetWindowProcessExePath(hwnd);

    handleProfileActivation(savedPath);
}

void Controller::run ()
{
    String previousPath;
    while (threadShouldExit() == false)
    {
        if (wait(Settings::getInstance()->getInterval()))
        {
            break;
        }

        auto path = WindowHelper::GetActiveWindowPath();

        if (path != previousPath)
        {
            previousPath = path;

            handleProfileActivation(path);
        }
    }
}

void Controller::addMapping ( bool create_default_mapping )
{
    auto m = std::make_unique<Mapping>();

    // Mapping m;
    m->deviceId(Mapping::devices().back().DeviceId());
    m->path(create_default_mapping ? "DEFAULT" : "empty");
    m->profileName(Mapping::devices().back().ProfileInfos.front().Name);
    m->isActive(true);

    _mappings.emplace_back(std::move(m));

    DBG(debugMappings());
}

void Controller::stop ()
{
    stopThread(_interval);
    WindowHelper::UnRegisterWindowEvents();
    sendActionMessage(StopMsg);
    log("Stopped");
}

void Controller::start ()
{
    const auto method = Settings::getInstance()->getMethod();
    if (method == Settings::DetectionMethod::Polling)
    {
        startThread(Priority::low);
    }
    else
    {
        WindowHelper::RegisterWindowEvents();
    }
    
    sendActionMessage(StartMsg);
    log("Running");
}
