#include "device.h"


namespace DiskManager {

Device::Device()
{
    qDBusRegisterMetaType<DeviceInfo>();
    qDBusRegisterMetaType<DeviceInfoMap>();
    qDBusRegisterMetaType<stCustest>();
}

}
