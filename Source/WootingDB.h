#pragma once

#include "Main.h"

class WootingDB
{
public:
    static juce::String findWootingLevelDb();
    static ProfilesByDevice retrieveProfileData();

private:
    static ProfilesByDevice extractProfileInfo(const juce::String& json);
};

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


