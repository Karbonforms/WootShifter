#include "Device.h"

ProfileInfo::ProfileInfo(const String& name, const String& uid, const String& device_id, const int idx)
{
    Name = name;
    Uid = uid;
    DeviceId = device_id;
    index = idx;   
}
