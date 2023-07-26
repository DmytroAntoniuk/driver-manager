#include "pch.h"

#include "device.h"

#include <memory>
#include <vector>

Device::Device(const HDEVINFO h_dev_info, const DWORD member_index)
{
    std::unique_ptr<SP_DEVINFO_DATA> device_info_data_ptr = std::make_unique<SP_DEVINFO_DATA>();
    SP_DEVINFO_DATA* device_info_data_loc                 = device_info_data_ptr.get();

    device_info_data_loc->cbSize = sizeof(SP_DEVINFO_DATA);

    if (!SetupDiEnumDeviceInfo(h_dev_info, member_index, device_info_data_loc))
    {
        LOG_ERROR("Error: {}", GetLastError());
        return;
    }

    DWORD buffersize = 0;
    SetupDiGetDeviceInstanceId(h_dev_info, device_info_data_loc, nullptr, 0, &buffersize);

    std::vector<wchar_t> instance_id_buffer(buffersize);

    if (!SetupDiGetDeviceInstanceId(h_dev_info, device_info_data_loc, instance_id_buffer.data(), buffersize, nullptr))
    {
        LOG_ERROR("Error: {}", GetLastError());
        return;
    }

    // Store the InstanceId in the class member
    InstanceId = static_cast<ofst>(state->textas.alloc(buffersize));
    wcscpy_s(state->textas.getw(InstanceId), buffersize, instance_id_buffer.data());

    // Use a map to store the properties with their respective SPDRP_ constants
    std::map<ULONG, std::wstring*> propertiesMap = {
        {SPDRP_DEVICEDESC, &Devicedesc},
        {SPDRP_HARDWAREID, &HardwareID},
        {SPDRP_COMPATIBLEIDS, &CompatibleIDs},
        {SPDRP_DRIVER, &Driver},
        {SPDRP_MFG, &Mfg},
        {SPDRP_FRIENDLYNAME, &FriendlyName},
        {SPDRP_CAPABILITIES, &Capabilities},
        {SPDRP_CONFIGFLAGS, &ConfigFlags}};

    // Read device properties and store them in the respective class members
    for (const auto& prop : propertiesMap)
    {
        read_device_property(h_dev_info, state, prop.first, prop.second);
    }

    ret = CM_Get_DevNode_Status(&status, &problem, device_info_data_loc->DevInst, 0);
    if (ret != CR_SUCCESS)
    {
        Log.print_err("ERROR %d with CM_Get_DevNode_Status()\n", ret);
    }
}
