#include "scanner.h"

// clang-format off
#include "Windows.h"
#include <fstream>
#include <prsht.h>
#include <setupapi.h>
#include <iostream>
// clang-format on

#define BUFLEN 4096

void Scanner::Run()
{
    std::cout << "scanner run" << std::endl;
    PVOID hDevInfo;
    HKEY hkey;
    wchar_t buf[BUFLEN];
    // Collection collection{textas.getw(windir), L"", L""};
    // Driverpack unpacked_drp{L"", L"windir.7z", &collection};

    // Timers.start(time_devicescan);

    hDevInfo = SetupDiGetClassDevs(nullptr, nullptr, nullptr, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        Log.print_syserr(GetLastError(), L"SetupDiGetClassDevs()");
        return;
    }

    unsigned DeviceCount = 0;
    for (unsigned i = 0;; i++)
    {
        // Device
        Devices_list.emplace_back((Device(hDevInfo, this, i)));
        Device *cur_device = &Devices_list.back();

        int ret = cur_device->getRet();
        if (ret)
        {
            Devices_list.pop_back();
            if (ret == ERROR_NO_MORE_ITEMS)
                break;
            else
                continue;
        }

        // Driver
        if (!cur_device->getDriver())
            continue;
        wsprintf(buf, L"SYSTEM\\CurrentControlSet\\Control\\Class\\%s", textas.getw(cur_device->getDriver()));
        Log.print_debug("State::scanDevices::Driver::%S\n", buf);
        ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, buf, 0, KEY_QUERY_VALUE, &hkey);
        switch (ret)
        {
            case ERROR_SUCCESS:
                cur_device->setDriverIndex(Drivers_list.size());
                Drivers_list.emplace_back(Driver(this, cur_device, hkey, &unpacked_drp));
                break;

            default:
                Log.print_syserr(ret, L"RegOpenKeyEx()");

            case ERROR_FILE_NOT_FOUND:
                break;
        }
        RegCloseKey(hkey);
        DeviceCount++;
    }

    Log.print_debug("State::scanDevices::Count::%d\n", DeviceCount);
    Log.print_debug("State::scanDevices::SetupDiDestroyDeviceInfoList\n");
    if (SetupDiDestroyDeviceInfoList(hDevInfo))
        Log.print_debug("State::scanDevices::SetupDiDestroyDeviceInfoList::Success\n");
    else
    {
        DWORD error = GetLastError();
        Log.print_debug("State::scanDevices::SetupDiDestroyDeviceInfoList::Error:%d\n", error);
    }
    Timers.stop(time_devicescan);
    Log.print_debug("State::scanDevices::Done\n");
}
