#pragma once
#include <vector>

class Driver
{
public:
    Driver() = default;
    Driver(HKEY hkey);
    void LoadProps();

private:
    void ReadRegistryValue(const std::wstring& value);

private:
    HKEY _hkey;

    std::wstring _driver_desc;
    std::wstring _provider_name;
    std::wstring _driver_date;
    std::wstring _driver_version;
    std::wstring _matching_device_id;
    std::wstring _inf_path;
    std::wstring _inf_section;
    std::wstring _inf_section_ext;
};
