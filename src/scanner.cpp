#include "pch.h"

#include "scanner.h"

#include "state.h"

void Scanner::Start()
{
    LOG_DEBUG("start scan");
    HDEVINFO h_device_info = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (h_device_info == INVALID_HANDLE_VALUE)
    {
        LOG_ERROR("SetupDiGetClassDevs failed. Error code: {}", GetLastError());
        return;
    }

    SP_DEVINFO_DATA device_info_data;
    device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD index = 0; SetupDiEnumDeviceInfo(h_device_info, index, &device_info_data); ++index)
    {
        Device current_device = State::GetInstance().AddDevice(h_device_info, device_info_data);
        if (current_device.GetDriver().empty())
            continue;
    }

    SetupDiDestroyDeviceInfoList(h_device_info);
    return;
}
