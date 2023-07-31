#pragma once

class Device
{
public:
    Device(HDEVINFO &h_dev_info, SP_DEVINFO_DATA &dev_info_data);

private:
    void ReadProperty(const DWORD id);

private:
    HDEVINFO _h_dev_info;
    SP_DEVINFO_DATA _dev_info_data;

    std::wstring _device_desc;
    std::wstring _hardware_id;
    std::wstring _compatible_ids;
    std::wstring _driver;
    std::wstring _mfg;
    std::wstring _friendly_name;
    std::wstring _capabilities;
    std::wstring _config_flags;
};

using Devices = std::vector<Device>;
