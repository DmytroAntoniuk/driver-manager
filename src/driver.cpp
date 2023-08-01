#include "pch.h"

#include "driver.h"

Driver::Driver(HKEY hkey) : _hkey(hkey)
{
    LoadProps();
    // calc_dev_pos(cur_device, state, &ishw, &dev_pos);

    // if (InfPath)
    //     scaninf(state, unpacked_drp, inf_pos);

    // identifierscore = calc_identifierscore(dev_pos, ishw, inf_pos);
    //// log_file("%d,%d,%d,(%x)\n",dev_pos,ishw,inf_pos,identifierscore);

    // if (DriverDate)
    //{
    //     wsprintfA(bufa, "%ws", state->textas.getw(DriverDate));
    //     Parser pi{bufa, bufa + strlen(bufa)};
    //     pi.readDate(&version);
    // }

    // if (DriverVersion)
    //{
    //     wsprintfA(bufa, "%ws", state->textas.getw(DriverVersion));
    //     Parser pi{bufa, bufa + strlen(bufa)};
    //     pi.readVersion(&version);
    // }
}

void Driver::LoadProps()
{
    std::array<std::wstring, 8> props = {L"DriverDesc", L"ProviderName", L"DriverDate", L"DriverVersion", L"MatchingDeviceId", L"InfPath", L"InfSection", L"InfSectionExt"};
    for (const auto& prop : props)
    {
        ReadRegistryValue(prop);
    }
}

void Driver::ReadRegistryValue(const std::wstring& value)
{
    LOG_DEBUG("read registry value");

    std::map<std::wstring, std::wstring&> properties_map = {
        {L"DriverDesc", _driver_desc},
        {L"ProviderName", _provider_name},
        {L"DriverDate", _driver_date},
        {L"DriverVersion", _driver_version},
        {L"MatchingDeviceId", _matching_device_id},
        {L"InfPath", _inf_path},
        {L"InfSection", _inf_section},
        {L"InfSectionExt", _inf_section_ext}};

    auto it = properties_map.find(value);
    if (it == properties_map.end())
    {
        LOG_ERROR("Property not found");
        return;
    }

    DWORD type, size = 0;
    LSTATUS status;
    status = RegQueryValueEx(_hkey, value.c_str(), nullptr, nullptr, nullptr, &size);
    if (status == ERROR_FILE_NOT_FOUND)
    {
        LOG_ERROR("file not found");
        return;
    }

    if (status != ERROR_SUCCESS)
    {
        LOG_ERROR("RegQueryValueEx failed. status={}", status);
        return;
    }

    TCHAR buffer[MAX_PATH];
    status = RegQueryValueEx(_hkey, value.c_str(), nullptr, &type, (BYTE*)buffer, &size);
    if (status != ERROR_SUCCESS)
    {
        LOG_ERROR("RegQueryValueEx failed. status={}", status);
        return;
    }
    else
    {
        it->second = std::wstring(buffer);
    }
}
