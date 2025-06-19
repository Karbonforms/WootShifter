#include "WootingControl.h"
#include "Device.h"
#include "wooting-rgb-sdk.h"

using namespace juce;

static uint16_t ReadUInt16LittleEndian(const uint8_t* buffer)
{
    return ByteOrder::littleEndianShort(buffer);
    // std::array<uint8_t, 2> bytes = {buffer[0], buffer[1]};
    // return std::bit_cast<uint16_t>(bytes);
}

std::vector<Device> WootingControl::RetrieveConnectedDevices()
{
    auto devices = std::vector<Device>();

    if (!wooting_rgb_kbd_connected())
    {
        Logger::outputDebugString("No Wooting devices found");
        return devices;
    }
    
    uint8_t count = wooting_usb_device_count();
    
    for (uint8_t i = 0; i < count; i++)
    {
        wooting_usb_select_device(i);
        
        const WOOTING_USB_META* info = wooting_rgb_device_info();
            
        uint8_t buff[MAX_LENGTH];
            
        int response = wooting_usb_send_feature_with_response(buff, MAX_LENGTH, GET_SERIAL, 0, 0, 0, 2);


        uint16_t read_u_int16_little_endian = ReadUInt16LittleEndian(buff);
        
        if ( read_u_int16_little_endian != MAGIC_WORD 
          || response != MAX_LENGTH
          || buff[2] != GET_SERIAL)
        {
            continue;
        }

        Device device;
        if (Device::TryParse(buff, MAX_LENGTH,device))
        {
            device.ParsedOK = true;
            device.Info = *info;
            device.ModelName = info->model;
                
            devices.push_back(device);
        }
        else
        {
            devices.emplace_back();
        }
    }

    return devices;
}

uint8_t WootingControl::get_active_profile_index()
{
    uint8_t buff[MAX_LENGTH];

    auto response = wooting_usb_send_feature_with_response(buff, MAX_LENGTH, GET_CURRENT_KEYBOARD_PROFILE_INDEX, 0, 0, 0, 0);

    if (response == MAX_LENGTH)
    {
        return buff[ wooting_usb_use_v2_interface() ? 5 : 4];
    }

    return UINT8_MAX;
}

bool WootingControl::set_active_profile_index(const uint8_t idx)
{
    if (get_active_profile_index() != idx)
    {
        wooting_usb_send_feature(ACTIVATE_PROFILE, 0, 0, 0, idx);

        // Thread::sleep(250);
        //
        wooting_usb_send_feature(RELOAD_PROFILE, 0, 0, 0, idx);
        //
        // Thread::sleep(250);
        //
        wooting_usb_send_feature(WOOT_DEV_RESET_ALL, 0, 0, 0, 0);
        //
        // Thread::sleep(250);
        //
        wooting_usb_send_feature(REFRESH_RGB_COLORS, 0, 0, 0, idx);

        return true;
    }

    return false;
}
