#pragma once

#include "Main.h"

class WootingControl
{
    static constexpr int        MAX_LENGTH = UINT8_MAX + 1;
    static constexpr int        MAX_RGB_ROWS = 6;
    static constexpr int        MAX_RGB_COLS = 21;
    static constexpr uint8_t    GET_CURRENT_KEYBOARD_PROFILE_INDEX = 11;
    static constexpr uint8_t    GET_SERIAL = 3;
    static constexpr uint8_t    MAX_DEVICES = 10;
    static constexpr uint8_t    RELOAD_PROFILE = 7;
    static constexpr uint8_t    ACTIVATE_PROFILE = 23;
    static constexpr uint8_t    REFRESH_RGB_COLORS = 29;
    static constexpr uint8_t    WOOT_DEV_RESET_ALL = 32;
    static constexpr uint16_t   MAGIC_WORD = 56016;
    
public:
    static DeviceVector RetrieveConnectedDevices();
    static uint8_t get_active_profile_index();
    static bool setActiveProfileIndex( uint8_t deviceIndex, uint8_t profileIndex );
};
