#pragma once
#include <vector>

class Driver
{
public:
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

    struct Props
    {
        static inline const std::wstring DRIVER_DESC        = L"DriverDesc";
        static inline const std::wstring PROVIDER_NAME      = L"ProviderName";
        static inline const std::wstring DRIVER_DATE        = L"DriverDate";
        static inline const std::wstring DRIVER_VERSION     = L"DriverVersion";
        static inline const std::wstring MATCHING_DEVICE_ID = L"MatchingDeviceId";
        static inline const std::wstring INF_PATH           = L"InfPath";
        static inline const std::wstring INF_SECTION        = L"InfSection";
        static inline const std::wstring INF_SECTION_EXT    = L"InfSectionExt";
    };
};
