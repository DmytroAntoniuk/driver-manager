#include "pch.h"

#include <iostream>

int main()
{
    HDEVINFO hDeviceInfoSet = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (hDeviceInfoSet == INVALID_HANDLE_VALUE)
    {
        std::cerr << "SetupDiGetClassDevs failed. Error code: " << GetLastError() << std::endl;
        return 1;
    }

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD index = 0; SetupDiEnumDeviceInfo(hDeviceInfoSet, index, &deviceInfoData); ++index)
    {
        TCHAR friendlyName[256];
        if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, &deviceInfoData, SPDRP_FRIENDLYNAME, NULL, (BYTE*)friendlyName, sizeof(friendlyName), NULL))
        {
            std::wcout << L"Device " << index << L": " << friendlyName << std::endl;
        }
        else
        {
            std::wcout << L"Can't get device" << std::endl;
        }

        TCHAR driverDescription[256];
        if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, &deviceInfoData, SPDRP_DRIVER, NULL, (BYTE*)driverDescription, sizeof(driverDescription), NULL))
        {
            std::wcout << L"Driver: " << driverDescription << std::endl;
        }
        else
        {
            std::wcout << L"Can't get driver" << std::endl;
        }
    }

    SetupDiDestroyDeviceInfoList(hDeviceInfoSet);
    return 0;
}
