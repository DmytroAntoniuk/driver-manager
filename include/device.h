#pragma once

class Device
{
public:
    Device(HDEVINFO h_dev_info);

private:
    void ReadProperty(const DWORD id);

private:
    HDEVINFO _h_dev_info;

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
