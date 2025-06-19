#pragma once
#include "Main.h"

#include <wooting-usb.h>
#define DEVICE_H_INCLUDED
// using namespace juce;
// using Windows::Foundation::Collections::IVector;
using juce::String;

struct ProfileInfo
{
    String Name;
    String Uid;
    String DeviceId;
    String ToString()
    {
        return Name;
    }
    int index = 0;
    ProfileInfo() = default;
    ProfileInfo(const String& name, const String& uid, const String& device_id, const int index);
};

class Device
{
    enum class Stage
    {
        H = 0, // Mass
        P = 1, // PVT
        T = 2, // DVT
        E = 3, // EVT
        X = 4 // Prototype
    };
public:
    String _device_id = L"";
    String ModelName = L"";
    uint32_t Supplier = 0;
    uint32_t Year = 0;
    uint32_t Week = 0;
    uint32_t Product = 0;
    uint32_t Revision = 0;
    uint32_t ProductId = 0;
    uint32_t DeviceStage = 0;
    uint32_t Variant = 0;
    uint32_t PcbDesign = 0;
    uint32_t MinorRev = 0;
    WOOTING_USB_META Info;

    String DeviceId()
    {
        return _device_id.isEmpty() ? GetDeviceId() : _device_id;
    }

    void DeviceId(String const& value)
    {
        _device_id = value;
    }

    bool ParsedOK = false;


    std::vector<ProfileInfo> ProfileInfos;


    // Static factory method for parsing
    static bool TryParse(uint8_t buffer[], size_t buffer_len, Device& device)
    {
        // device = null;
        // try
        // {
        constexpr int OFFSET = 5;

        if (buffer_len < OFFSET)
            return false;

        const uint32_t length = buffer[4];
        uint32_t index = OFFSET;
        device = Device();

        while (index < length + OFFSET)
        {
            uint8_t field = buffer[index];
            index++;

            // Field number is in the upper 5 bits
            int field_number = field >> 3;

            switch (field_number)
            {
            case 1: // supplier
                uint32_t supplier;
                if (!TryParseVarInt(buffer, buffer_len, index, supplier))
                    return false;
                device.Supplier = supplier;
                break;

            case 2: // year
                uint32_t year;
                if (!TryParseVarInt(buffer, buffer_len, index, year))
                    return false;
                device.Year = year;
                break;

            case 3: // week
                uint32_t week;
                if (!TryParseVarInt(buffer, buffer_len, index, week))
                    return false;
                device.Week = week;
                break;

            case 4: // product
                uint32_t product;
                if (!TryParseVarInt(buffer, buffer_len, index, product))
                    return false;
                device.Product = product;
                break;

            case 5: // revision
                uint32_t revision;
                if (!TryParseVarInt(buffer, buffer_len, index, revision))
                    return false;
                device.Revision = revision;
                break;

            case 6: // product_id
                uint32_t product_id;
                if (!TryParseVarInt(buffer, buffer_len, index, product_id))
                    return false;
                device.ProductId = product_id;
                break;

            case 7: // stage
                uint32_t stage_value;
                if (!TryParseVarInt(buffer, buffer_len, index, stage_value))
                    return false;
                device.DeviceStage = stage_value;
                break;

            case 9: // variant
                uint32_t variant;
                if (!TryParseVarInt(buffer, buffer_len, index, variant))
                    return false;
                device.Variant = variant;
                break;

            case 10: // pcb_design
                uint32_t pcb_design;
                if (!TryParseVarInt(buffer, buffer_len, index, pcb_design))
                    return false;
                device.PcbDesign = pcb_design;
                break;

            case 11: // minor_rev
                uint32_t minor_rev;
                if (!TryParseVarInt(buffer, buffer_len, index, minor_rev))
                    return false;
                device.MinorRev = minor_rev;
                break;

            default: // Skip unknown fields
                int wire_type = field & 7;
                if (!SkipField(buffer, buffer_len, index, wire_type))
                    return false;
                break;
            }
        }
        return true;
        // }
        // catch (Exception)
        // {
        //     return false;
        // }
    }

    static bool TryParseVarInt(uint8_t buffer[], size_t len, uint32_t& index, uint32_t& value)
    {
        value = 0;
        uint32_t result = 0;
        int shift = 0;

        while (index < len)
        {
            uint8_t b = buffer[index];
            result |= (uint32_t)((b & 0x7F) << shift);
            index++;

            if ((b & 0x80) == 0)
            {
                value = result;
                return true;
            }

            shift += 7;
            if (shift > 28)
                return false;
        }

        return false;
    }

    static bool SkipField(uint8_t buffer[], size_t len, uint32_t& index, int wire_type)
    {
        // try
        // {
        switch (wire_type)
        {
        case 0:
            {
                // Varint
                index++;
                break;
            }
        case 1:
            {
                // 64-bit
                index += 8;
                break;
            }
        case 2:
            {
                // Length-delimited
                if (index >= len)
                    return false;
                int length = buffer[index];
                index += length + 1;
                break;
            }
        case 3:
            {
                // Start group (deprecated in protobuf)
                while (index < len && (buffer[index] & 7) != 4)
                    index++;
                index++;
                break;
            }
        case 5:
            {
                // 32-bit
                index += 4;
                break;
            }
        default:
            {return false;}
        }

        return true;
        // }
        // catch (Exception)
        // {
        //     return false;
        // }
    }

    // Additional methods for generating device IDs and serial numbers
    String GetDeviceId()
    {
        if (_device_id.isEmpty())
        {
            String keyboard_type;
            if (ModelName == L"Wooting One")
                keyboard_type = L"0";
            else if (ModelName == L"Wooting Two")
                keyboard_type = L"1";
            else if (ModelName == L"Wooting Two LE")
                keyboard_type = L"2";
            else if (ModelName == L"Wooting Two HE")
                keyboard_type = L"3";
            else if (ModelName == L"Wooting 60HE")
                keyboard_type = L"4";
            else if (ModelName == L"Wooting 60HE (ARM)")
                keyboard_type = L"5";
            else if (ModelName == L"Wooting Two HE (ARM)")
                keyboard_type = L"6";
            else if (ModelName == L"Wooting UwU")
                keyboard_type = L"7";
            else if (ModelName == L"Wooting UwU RGB")
                keyboard_type = L"8";
            else if (ModelName == L"Wooting 60HE+")
                keyboard_type = L"9";
            else if (ModelName == L"Wooting 80HE")
                keyboard_type = L"10";
            else
                keyboard_type = L"NotFound";


            _device_id << keyboard_type
                       << String(ProductId)
                       << String(Product)
                       << String(Revision)
                       << String(Week)
                       << String(Year)
                       << String(PcbDesign)
                       << String(MinorRev)
                       << String(Variant);
        }

        return _device_id;
    }

    String GetSerialNumber()
    {
        String pcb_design = "T" + String::formatted("{:02}", PcbDesign);
        String minor_rev = String::formatted("{:02}", MinorRev);
        String variant = String::formatted("{:02}", Variant);


        return String::formatted("A{:02X}B{:02d}{:02d}W{:02X}{}{}{}{}{}{:05d}",
                                      Supplier,
                                      Year,
                                      Week,
                                      Product,
                                      pcb_design,
                                      Revision,
                                      minor_rev,
                                      variant,
                                      DeviceStage,
                                      ProductId);
    }
};
