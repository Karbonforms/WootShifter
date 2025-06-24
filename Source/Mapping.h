#pragma once

#include "Device.h"
#include "WindowHelper.h"

struct DeviceDisplay
{
    String DisplayName;
    String DeviceId;
};

class Mapping final : public juce::ChangeBroadcaster
{
public:
    Mapping();

    String toString() const;

    // Properties
    String deviceId() const { return _deviceId; }
    void deviceId(String const& value);

    Device& device() { return _device; }
    String deviceName() const { return _device.ModelName; }

    String path() const { return _path; }
    void path(String const& value);

    String profileName() const { return _profileName; }
    void profileName(String const& value);

    uint8_t profileIndex() const { return _profileIdx; }
    
    bool isActive() const { return _active; }
    void isActive(bool value) { _active = value; }

    // Computed properties
    bool isActualPath() const;
    bool fileExists() const;
    bool isDefault() const { return _path == L"DEFAULT"; }
    bool isNotDefault() const { return !isDefault(); }

    // Collections
    std::vector<String> deviceProfiles() const { return _deviceProfiles; }
    void deviceProfiles(std::vector<String> const& value);

    std::vector<DeviceDisplay>& deviceIds() { return _deviceIds; }
    void deviceIds(std::vector<DeviceDisplay> const& value);

    // Static members
    static std::vector<Device>& devices() { return _devices; }
    static void devices(std::vector<Device> const& value) { _devices = value; }
    static juce::Image GetDefaultIcon();

    
    juce::Image exeIcon() const;

    void initialize();

private:

    String      _deviceId {L"NO DEVICE ID"};
    String      _path {L"NO EXE PATH"};
    String      _profileName {L"NO PROFILE"};
    bool        _active {true};
    uint8_t     _profileIdx;
    Device      _device;
    juce::Image _icon;
    
    std::vector<String> _deviceProfiles;
    std::vector<DeviceDisplay> _deviceIds;
    
    static std::vector<Device> _devices;
};
