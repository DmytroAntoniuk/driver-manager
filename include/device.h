#pragma once

class Device
{
public:
    Device(HDEVINFO h_dev_info, const DWORD member_index);

private:
    void ReadProperty(const int id, unsigned& value);

private:
    HDEVINFO _h_dev_info;
    std::wstring _instance_id;
    PSP_DEVINFO_DATA _device_info_data;

    std::wstring _device_desc;
    std::wstring _hardware_id;
    std::wstring _compatible_ids;
    std::wstring _driver;
    std::wstring _mfg;
    std::wstring _friendly_name;
    std::wstring _capabilities;
    std::wstring _config_flags;
};
