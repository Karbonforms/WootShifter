#pragma once

#include "Main.h"

// Define the KEY constant
const std::vector<uint8_t> KEY = []()
{
    std::string prefix = "_file://";
    std::string suffix = "persist:root";
    std::vector<uint8_t> key;

    // Add prefix
    key.insert(key.end(), prefix.begin(), prefix.end());

    // Add \0\1
    key.push_back(0x00);
    key.push_back(0x01);

    // Add suffix
    key.insert(key.end(), suffix.begin(), suffix.end());

    return key;
}();

class WootingDB
{
public:
    static juce::String findWootingLevelDb()
    {
        const auto roaming = juce::File::getSpecialLocation(juce::File::windowsLocalAppData).getSiblingFile("roaming");
        const auto results = roaming.findChildFiles(juce::File::TypesOfFileToFind::findDirectories, false, "wootility*", juce::File::FollowSymlinks::no);
        for (const auto& result : results)
        {
            const auto levelDbDir = result.getChildFile("Local Storage/leveldb");
            if (levelDbDir.exists())
            {
                auto& rval = levelDbDir.getFullPathName();
                juce::Logger::outputDebugString("Found wootility: " + rval);
                return rval;
            }
        }
        return {};
        //return R"(C:\Users\klf\AppData\Roaming\wootility\Local Storage\leveldb)";
    }

    static ProfilesByDevice retrieveProfileData();

private:
    static ProfilesByDevice extractProfileInfo(const juce::String& json);
};
