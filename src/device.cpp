#include "pch.h"

#include "device.h"

#include <memory>

Device::Device(HDEVINFO &h_dev_info, SP_DEVINFO_DATA &dev_info_data) : _h_dev_info(h_dev_info), _dev_info_data(dev_info_data)
{
    LOG_DEBUG("ctor");
    const std::array<DWORD, 8> props = {
        SPDRP_DEVICEDESC, SPDRP_HARDWAREID, SPDRP_COMPATIBLEIDS, SPDRP_DRIVER, SPDRP_MFG, SPDRP_FRIENDLYNAME, SPDRP_CAPABILITIES, SPDRP_CONFIGFLAGS};

    for (const auto &prop : props)
    {
        ReadProperty(prop);
    }
}

void Device::ReadProperty(const DWORD id)
{
    LOG_DEBUG("read property {}", id);

    std::map<DWORD, std::wstring &> properties_map = {
        {SPDRP_DEVICEDESC, _device_desc},
        {SPDRP_HARDWAREID, _hardware_id},
        {SPDRP_COMPATIBLEIDS, _compatible_ids},
        {SPDRP_DRIVER, _driver},
        {SPDRP_MFG, _mfg},
        {SPDRP_FRIENDLYNAME, _friendly_name},
        {SPDRP_CAPABILITIES, _capabilities},
        {SPDRP_CONFIGFLAGS, _config_flags}};

    auto it = properties_map.find(id);
    if (it == properties_map.end())
    {
        LOG_ERROR("Property not found");
        return;
    }

    TCHAR buffer[256];
    if (SetupDiGetDeviceRegistryProperty(_h_dev_info, &_dev_info_data, id, NULL, (BYTE *)buffer, sizeof(buffer), NULL))
    {
        it->second = std::wstring(buffer);
        LOG_DEBUG("Device : {}", StringHelper::Converter::ToString(buffer));
    }
    else
    {
        LOG_ERROR("Can't get device");
    }
}
