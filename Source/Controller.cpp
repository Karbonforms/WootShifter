#include "Controller.h"
#include "Mapping.h"
#include "Settings.h"
#include "WootingControl.h"
#include "WootingDB.h"

#define JSON_NOEXCEPTION 1
#include "json.hpp"

using namespace juce;
using namespace nlohmann;

JUCE_IMPLEMENT_SINGLETON(Controller);

Controller::Controller()
: Thread("Controller")
{
    Mapping::devices(WootingControl::RetrieveConnectedDevices());

    _currentDeviceId = Mapping::devices().back().DeviceId();

    for (auto& device : Mapping::devices())
    {
        Logger::outputDebugString(device.ModelName);
        // mainWindow->getContentComponent()
        // log()
    }

    const auto profilesByDevice = WootingDB::retrieveProfileData();

    if (profilesByDevice.empty())
    {
        JUCEApplication::quit();
    }
    
    assignProfilesToDevices(profilesByDevice);

    const auto appdir = Settings::getApplicationDataDirectory();

    const auto mappingsFile = appdir.getChildFile("mappings.json");
    
    if (mappingsFile.existsAsFile())
    {
        auto jsonString = mappingsFile.loadFileAsString().toStdString();

        if (!json::accept(jsonString))
        {
            Logger::outputDebugString("Invalid JSON");
            AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Invalid JSON", "Invalid JSON in mappings.json");
            JUCEApplication::quit();
            return;
        }
        
        json jmappings = json::parse(jsonString);

        for (const auto& mapping : jmappings)
        {
            auto m = std::make_unique<Mapping>();

            m->deviceId(std::string(mapping["DeviceId"]));
            m->path(std::string(mapping["Path"]));
            m->profileName(std::string(mapping["Profile"]));
            m->isActive(mapping["IsActive"]);

            _mappings.emplace_back(std::move(m));
        }

        if (_mappings.empty())
            addMapping(true);

        std::ranges::sort(_mappings, [](const auto& a, const auto& b)
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
}

void Controller::assignProfilesToDevices(const ProfilesByDevice& profilesByDevice)
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

void Controller::setLogout(TextEditor* editor)
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

        // data << "\n\nDevice profiles:\n";
        // for (auto const& [key, val] : profilesByDevice)
        // {
        //     data << key << "\n\t";
        //     for (auto const& profile : val)
        //     {
        //         data << profile.Name << ' ' << profile.Uid << '\n';
        //     }
        //     data << '\n';
        // }
        log(data);
    }
}

void Controller::saveMappings() const
{
    if (_mappings.empty())
        return;

    json jmappings = json::array();

    for (const auto& mapping : _mappings)
    {
        json j;
        j["DeviceId"] = mapping->deviceId().toStdString();
        j["Path"] = mapping->path().toStdString();
        j["Profile"] = mapping->profileName().toStdString();
        j["IsActive"] = mapping->isActive();

        jmappings.push_back(j);
    }

    auto appdir = File::getSpecialLocation(File::windowsLocalAppData).getChildFile(
        JUCEApplication::getInstance()->getApplicationName());
    if (appdir.exists() == false)
    {
        const auto r = appdir.createDirectory();
        if (r.failed())
        {
            Logger::outputDebugString("Failed to create directory: " + appdir.getFullPathName());
            return;
        }
    }

    auto mappings_file = appdir.getChildFile("mappings.json");
    if (mappings_file.replaceWithText(jmappings.dump(4)))
    {
        Logger::outputDebugString("Saved mappings");
    }
}

void Controller::log(String const& msg) const
{
    if (_logout)
    {
        // auto string = logout->getText();
        // string.append(msg, msg.length());
        // logout->setText(string, false);

        _logout->insertTextAtCaret(msg);
        _logout->insertTextAtCaret("\n");
    }
}

Controller::~Controller()
{
    stopThread(_interval);
    saveMappings();
}

void Controller::run()
{
    String previous_path;
    while (threadShouldExit() == false)
    {
        if (wait(Settings::getInstance()->getInterval()))
        {
            break;
        }

        auto path = WindowHelper::getActiveWindowPath();

        if (path != previous_path)
        {
            previous_path = path;

            auto pred = [&path](const std::unique_ptr<Mapping>& m)
            {
                return m->path() == path && m->isActive();
            };

            auto mappingIterator = std::ranges::find_if(_mappings, pred);

            if (mappingIterator != _mappings.end())
            {
                const auto i = mappingIterator->get()->profileIndex();

                if (WootingControl::set_active_profile_index(i))
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
                        const auto i = mapping->profileIndex();
                        if (WootingControl::set_active_profile_index(i))
                        {
                            const MessageManagerLock lock;
                            log("Active profile: " + mapping->profileName());
                        }

                        break;
                    }
                }
            }
        }
    }
}

void Controller::addMapping(bool create_default_mapping)
{
    auto m = std::make_unique<Mapping>();

    // Mapping m;
    m->deviceId(Mapping::devices().back().DeviceId());
    m->path(create_default_mapping ? "DEFAULT" : "empty");
    m->profileName(Mapping::devices().back().ProfileInfos.front().Name);
    m->isActive(false);

    _mappings.emplace_back(std::move(m));
}

void Controller::stop ()
{
    stopThread(_interval);
    sendActionMessage("Stopped.");
}

void Controller::start ()
{
    startThread(Priority::low);
    sendActionMessage("Running!");
}
