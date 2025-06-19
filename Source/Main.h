#pragma once
#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4100)  // unreferenced formal parameter
    #pragma warning(disable: 4840)  // non-portable use of class as an argument to variadic function
#endif

#define JUCE_MODAL_LOOPS_PERMITTED 1
#include <JuceHeader.h>

#ifdef _MSC_VER
    #pragma warning(pop)
#endif

#include <map>
#include <vector>

// #include "Device.h"
// #include "Mapping.h"

class Device;
struct ProfileInfo;
class Mapping;

using ProfilesByDevice = std::map<juce::String, std::vector<ProfileInfo>>;
using DeviceVector = std::vector<Device>;
using Mappings = std::vector<std::unique_ptr<Mapping>>;

