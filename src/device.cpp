#include "pch.h"

#include "device.h"

#include <memory>

Device::Device(HDEVINFO h_dev_info, SP_DEVINFO_DATA& dev_info_data) : _h_dev_info(h_dev_info), _dev_info_data(dev_info_data)
{
    LOG_DEBUG("ctor");

    LoadProps();
    LoadDriver();
}

void Device::LoadProps()
{
    LOG_DEBUG("init props");

    const std::array<DWORD, 8> props = {
        SPDRP_DEVICEDESC, SPDRP_HARDWAREID, SPDRP_COMPATIBLEIDS, SPDRP_DRIVER, SPDRP_MFG, SPDRP_FRIENDLYNAME, SPDRP_CAPABILITIES, SPDRP_CONFIGFLAGS};

    for (const auto& prop : props)
    {
        ReadProperty(prop);
    }
}

void Device::LoadDriver()
{
    LOG_DEBUG("init driver");

    if (_driver_id.empty())
    {
        LOG_ERROR("driver for {} not found", GetDeviceDesc());
        return;
    }

    HKEY hkey;
    std::wstring subkey = std::format(L"SYSTEM\\CurrentControlSet\\Control\\Class\\{}", StringHelper::Converter::ToString(_driver_id));
    int ret             = RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey.c_str(), 0, KEY_QUERY_VALUE, &hkey);
    switch (ret)
    {
        case ERROR_SUCCESS:
            _driver = Driver{hkey};
            break;

        default:
            LOG_DEBUG("RegOpenKeyEx. ret={}", ret);

        case ERROR_FILE_NOT_FOUND:
            break;
    }

    RegCloseKey(hkey);
}

void Device::ReadProperty(const DWORD id)
{
    LOG_DEBUG("read property {}", id);

    std::map<DWORD, std::wstring&> properties_map = {
        {SPDRP_DEVICEDESC, _device_desc},
        {SPDRP_HARDWAREID, _hardware_id},
        {SPDRP_COMPATIBLEIDS, _compatible_ids},
        {SPDRP_DRIVER, _driver_id},
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
    if (SetupDiGetDeviceRegistryProperty(_h_dev_info, &_dev_info_data, id, NULL, (BYTE*)buffer, sizeof(buffer), NULL))
    {
        it->second = std::wstring(buffer);
        LOG_DEBUG("Device : {}", StringHelper::Converter::ToString(buffer));
    }
    else
    {
        LOG_ERROR("Can't get device");
    }
}

const std::wstring& Device::GetDeviceDesc() const { return _device_desc; }

const std::wstring& Device::GetHardwareId() const { return _hardware_id; }

const std::wstring& Device::GetCompatibleIds() const { return _compatible_ids; }

const std::wstring& Device::GetDriverId() const { return _driver_id; }

const std::wstring& Device::GetMfg() const { return _mfg; }

const std::wstring& Device::GetFriendlyName() const { return _friendly_name; }

const std::wstring& Device::GetCapabilities() const { return _capabilities; }

const std::wstring& Device::GetConfigFlags() const { return _config_flags; }
