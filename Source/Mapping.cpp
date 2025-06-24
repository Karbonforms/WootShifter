#include "Mapping.h"

#include "juce_gui_basics/detail/juce_WindowingHelpers.h"

using namespace juce;
using namespace juce::detail;

std::vector<Device> Mapping::_devices;

Mapping::Mapping() = default;

String Mapping::toString () const
{
    String result;
    result << "Mapping: " << deviceName() << " : " << profileName() << " : " << path() << newLine;
    return result;
}

void Mapping::deviceId(String const& value)
{
    if (_deviceId == value) return;
    
    DBG(_deviceId + L" -> " + value);
    _deviceId = value;
    initialize();
    
    sendChangeMessage();
}

void Mapping::path(String const& value)
{
    if (_path == value) return;
    
    DBG(_path + L" -> " + value);

    _path = value;

    if (fileExists() && !isDefault())
    {
        _icon = WindowingHelpers::createIconForFile(_path);
    }
    else if (isDefault())
    {
        _icon = GetDefaultIcon();
    }

    sendChangeMessage();
}

void Mapping::profileName ( String const& value )
{
    if (_profileName == value) return;
    
    _profileName = value;
    uint8_t idx = 0;
    for (auto const& profile : deviceProfiles())
    {
        if (profile == _profileName)
        {
            _profileIdx = idx;
            return;
        }
        idx++;
    }
    
    sendChangeMessage();
}

void Mapping::initialize()
{
    if (_devices.empty()) return;
    
    auto newDevices = std::vector<DeviceDisplay>();

    for (auto& device : _devices)
    {
        auto deviceId = device.DeviceId();

        auto display = DeviceDisplay();
        display.DisplayName = device.ModelName;
        display.DeviceId = deviceId;
        newDevices.push_back(display);

        if (deviceId == _deviceId)
        {
            _device = device;
            auto profiles = std::vector<String>();
            for (auto const& profile : _device.ProfileInfos)
            {
                profiles.push_back(profile.Name);
            }
            deviceProfiles(profiles);
        }
    }

    deviceIds(newDevices);
}

bool Mapping::isActualPath() const
{
    return File::isAbsolutePath(_path);
}

bool Mapping::fileExists() const
{
    if (File::isAbsolutePath(_path))
    {
        return File(_path).existsAsFile();
    }

    return false;
}

void Mapping::deviceProfiles(std::vector<String> const& value)
{
    _deviceProfiles = value;
}

void Mapping::deviceIds(std::vector<DeviceDisplay> const& value)
{
    _deviceIds = value;
}

Image Mapping::GetDefaultIcon ()
{
    return WindowingHelpers::createIconForFile(File("C:\\Windows\\explorer.exe"));   
}

Image Mapping::exeIcon() const
{
    if (_path.isEmpty()) return {};

    if (_icon.isValid())
    {
        return _icon;
    }

    return {};
}
