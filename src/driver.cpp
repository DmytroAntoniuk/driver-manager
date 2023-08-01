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
    std::array<std::wstring, 8> props = {
        Props::DRIVER_DESC,
        Props::PROVIDER_NAME,
        Props::DRIVER_DATE,
        Props::DRIVER_VERSION,
        Props::MATCHING_DEVICE_ID,
        Props::INF_PATH,
        Props::INF_SECTION,
        Props::INF_SECTION_EXT};
    for (const auto& prop : props)
    {
        ReadRegistryValue(prop);
    }
}

void Driver::ReadRegistryValue(const std::wstring& value)
{
    LOG_DEBUG("read registry value");

    std::map<std::wstring, std::wstring&> properties_map = {
        {Props::DRIVER_DESC, _driver_desc},
        {Props::PROVIDER_NAME, _provider_name},
        {Props::DRIVER_DATE, _driver_date},
        {Props::DRIVER_VERSION, _driver_version},
        {Props::MATCHING_DEVICE_ID, _matching_device_id},
        {Props::INF_PATH, _inf_path},
        {Props::INF_SECTION, _inf_section},
        {Props::INF_SECTION_EXT, _inf_section_ext}};

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
