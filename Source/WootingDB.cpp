#include "WootingDB.h"
#include "Device.h"
#define JSON_NOEXCEPTION 1
#include "json.hpp"
#include "leveldb/db.h"

using namespace juce;

String WootingDB::findWootingLevelDb ()
{
    const auto roaming = File::getSpecialLocation(File::windowsLocalAppData).getSiblingFile("roaming");
    const auto results = roaming.findChildFiles(File::TypesOfFileToFind::findDirectories, false, "wootility*", File::FollowSymlinks::no);
    for (const auto& result : results)
    {
        const auto levelDbDir = result.getChildFile("Local Storage/leveldb");
        if (levelDbDir.exists())
        {
            auto& rval = levelDbDir.getFullPathName();
            DBG("Found wootility: " + rval);
            return rval;
        }
    }
    return {};
    //return R"(C:\Users\klf\AppData\Roaming\wootility\Local Storage\leveldb)";
}

ProfilesByDevice WootingDB::retrieveProfileData()
{
    // Open a connection to DB
    leveldb::Options options;
    options.create_if_missing = false;
    options.compression = leveldb::kSnappyCompression;
    options.paranoid_checks = true;

    leveldb::DB* db;
    leveldb::Status status = leveldb::DB::Open(options,
                                               findWootingLevelDb().toStdString(),
                                               &db);

    if (!status.ok())
    {
        auto title = "Failed to open Wootility database!";

        String msg;
        if (status.IsIOError())
        {
            msg << "Wootility must not be running!\n\n";
        }

        msg << "Error: " << status.ToString();
        
        AlertWindow::showMessageBox(AlertWindow::WarningIcon, title, msg);
        DBG(title + msg);
        return {};
    }
    
    std::string value;
    status = db->Get(leveldb::ReadOptions(),
                     leveldb::Slice(reinterpret_cast<const char*>(KEY.data()), KEY.size()),
                     &value);

    value = value.substr(1, value.size() - 1);

    auto profilesByDevice = extractProfileInfo(value);

    delete db;
    return profilesByDevice;
}

ProfilesByDevice WootingDB::extractProfileInfo(const String& json)
{
    ProfilesByDevice profiles_by_device;

    try
    {
        // using JsonDocument outer_doc = JsonDocument.Parse(json_content);
        // Parse the JSON string
        std::string json_content = json.toStdString();
        nlohmann::json outer_doc = nlohmann::json::parse(json_content);

        // string? profiles_json = outer_doc.RootElement.GetProperty("profiles").GetString();
        // Get the profiles string
        std::string profiles_json = outer_doc["profiles"].get<std::string>();

        // Parse the profiles JSON string
        nlohmann::json profiles_doc = nlohmann::json::parse(profiles_json);

        // Check device profiles
        if (profiles_doc.contains("devices"))
        {
            auto& devices = profiles_doc["devices"];

            // Iterate through devices
            for (auto& [device_id, device_profiles] : devices.items())
            {
                profiles_by_device[device_id] = std::vector<ProfileInfo>();

                // Iterate through profiles in device
                int index = 0;
                for (auto& profile : device_profiles)
                {
                    if (profile.contains("details"))
                    {
                        auto& details = profile["details"];
                        std::string name = details["name"].get<std::string>();
                        std::string uid = details["uid"].get<std::string>();
                        
                        profiles_by_device[device_id].emplace_back(name, uid, device_id, index++);
                    }
                }
            }
        }
    }
    catch (const nlohmann::json::exception& ex)
    {
        DBG(ex.what());
        // std::string("Failed to parse profiles: ") + ex.what());
    }

    return profiles_by_device;
}


// static String DecodeString(const std::vector<uint8_t>& bytes)
// {
//     if (bytes.empty())
//     {
//         throw std::invalid_argument("Invalid length");
//     }
//
//     uint8_t prefix = bytes[0];
//     std::vector<uint8_t> data(bytes.begin() + 1, bytes.end());
//
//     switch (prefix)
//     {
//     case 0: // UTF-16LE
//         // Note: You'll need to implement proper UTF-16LE conversion
//         //return std::string(reinterpret_cast<const char*>(data.data()), data.size());
//         return String::createStringFromData(data.data(), data.size());
//     case 1: //{ // Windows-1252
//             // Convert from Windows-1252 to UTF-8
//             // First convert to UTF-16
//             //int wideSize = MultiByteToWideChar(1252, 0, data, static_cast<int>(data.size()), nullptr, 0);
//             //if (wideSize > 0)
//             //{
//                 // std::vector<wchar_t> wideStr(wideSize);
//                 // MultiByteToWideChar(1252, 0, input, static_cast<int>(inputSize), wideStr.data(), wideSize);
//                 //
//                 // // Then convert from UTF-16 to UTF-8
//                 // int utf8Size = WideCharToMultiByte(CP_UTF8, 0, wideStr.data(), wideSize, nullptr, 0, nullptr, nullptr);
//                 // if (utf8Size > 0)
//                 // {
//                 //     result.resize(utf8Size);
//                 //     WideCharToMultiByte(CP_UTF8, 0, wideStr.data(), wideSize, &result[0], utf8Size, nullptr, nullptr);
//                 // }
//             //}
//             break;
//
//     default:
//         throw std::invalid_argument("Invalid prefix");
//     }
// }
