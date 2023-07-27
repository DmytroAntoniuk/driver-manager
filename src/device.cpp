#include "pch.h"

#include "device.h"

#include <memory>

Device::Device(HDEVINFO h_dev_info, const DWORD member_index) : _h_dev_info(h_dev_info)
{
    std::unique_ptr<SP_DEVINFO_DATA> device_info_data_ptr = std::make_unique<SP_DEVINFO_DATA>();
    SP_DEVINFO_DATA* device_info_data_loc                 = device_info_data_ptr.get();

    device_info_data_loc->cbSize = sizeof(SP_DEVINFO_DATA);

    if (!SetupDiEnumDeviceInfo(_h_dev_info, member_index, device_info_data_loc))
    {
        LOG_ERROR("Error: {}", GetLastError());
        return;
    }

    DWORD buffersize = 0;
    SetupDiGetDeviceInstanceId(_h_dev_info, device_info_data_loc, nullptr, 0, &buffersize);

    std::wstring instance_id_buffer;
    instance_id_buffer.resize(buffersize);

    if (!SetupDiGetDeviceInstanceId(_h_dev_info, device_info_data_loc, instance_id_buffer.data(), buffersize, nullptr))
    {
        LOG_ERROR("Error: {}", GetLastError());
        return;
    }

    LOG_DEBUG("Device Instance ID: {}", StringHelper::Converter::ToString(instance_id_buffer));

    _instance_id = instance_id_buffer;
    // LOG_DEBUG("Type = {}", typeid(SPDRP_DEVICEDESC).name());
    std::map<int, std::wstring> propertiesMap = {
        {SPDRP_DEVICEDESC, _device_desc},
        {SPDRP_HARDWAREID, _hardware_id},
        {SPDRP_COMPATIBLEIDS, _compatible_ids},
        {SPDRP_DRIVER, _driver},
        {SPDRP_MFG, _mfg},
        {SPDRP_FRIENDLYNAME, _friendly_name},
        {SPDRP_CAPABILITIES, _capabilities},
        {SPDRP_CONFIGFLAGS, _config_flags}};

    for (const auto& prop : propertiesMap)
    {
        // ReadProperty(prop.first, prop.second);
    }

    // ret = CM_Get_DevNode_Status(&status, &problem, device_info_data_loc->DevInst, 0);
    // if (ret != CR_SUCCESS)
    //{
    //     Log.print_err("ERROR %d with CM_Get_DevNode_Status()\n", ret);
    // }
}

void Device::ReadProperty(const int id, unsigned& value)
{
    // DWORD buffer_size = 0;
    // DWORD data_t      = 0;

    //// First, determine the buffer size needed
    // if (!SetupDiGetDeviceRegistryProperty(_h_dev_info, _device_info_data, id, &data_t, nullptr, 0, &buffer_size))
    //{
    //     LOG_ERROR("Error: {}", GetLastError());
    //     return;
    // }

    //// Allocate buffer using std::vector
    // std::vector<BYTE> buffer(buffer_size);

    // if (!SetupDiGetDeviceRegistryProperty(_h_dev_info, _device_info_data, id, &data_t, buffer.data(), buffer_size, &buffer_size))
    //{
    //     LOG_ERROR("Error: {}", GetLastError());
    //     return;
    // }

    //// If the property is REG_DWORD, copy directly into val
    // if (data_t == REG_DWORD && buffer_size >= sizeof(DWORD))
    //{
    //     value = *reinterpret_cast<DWORD*>(buffer.data());
    // }
    // else
    //{
    //     // If not REG_DWORD, allocate space in the 'state->textas' pool and copy the data
    //     value   = static_cast<ofst>(state->textas.alloc(buffer_size));
    //     BYTE* p = state->textas.get(*val);
    //     if (p != nullptr)
    //     {
    //         memcpy(p, buffer.data(), buffer_size);
    //     }
    //     else
    //     {
    //         Log.print_file("Failed to allocate memory for Property %d\n", id);
    //     }
    // }
}
