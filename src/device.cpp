#include "pch.h"

#include "device.h"
Device void Device::print_guid(const GUID *g)
{
    WString buffer;

    if (!SetupDiGetClassDescription(g, buffer.GetV(), static_cast<DWORD>(buffer.Length()), nullptr))
    {
        Log.print_file(
            "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
            g->Data1,
            g->Data2,
            g->Data3,
            g->Data4[0],
            g->Data4[1],
            g->Data4[2],
            g->Data4[3],
            g->Data4[4],
            g->Data4[5],
            g->Data4[6],
            g->Data4[7]);

        unsigned lr = GetLastError();
        if (lr != 0xE0000206)
            Log.print_syserr(lr, L"print_guid()");
    }
    Log.print_file("%S\n", buffer.Get());
}

void Device::read_device_property(HDEVINFO hDevInfo, State *state, int id, ofst *val)
{
    DWORD buffersize = 0;
    DWORD DataT      = 0;
    PBYTE p;
    auto DeviceInfoDataloc = (SP_DEVINFO_DATA *)&DeviceInfoData;

    *val = 0;
    if (!SetupDiGetDeviceRegistryProperty(hDevInfo, DeviceInfoDataloc, id, &DataT, nullptr, 0, &buffersize))
    {
        int ret_er = GetLastError();
        if (ret_er == ERROR_INVALID_DATA)
            return;
        if (ret_er != ERROR_INSUFFICIENT_BUFFER)
        {
            Log.print_file("Property %d\n", id);
            Log.print_syserr(ret_er, L"read_device_property()");
            return;
        }
    }

    if (DataT == REG_DWORD)
    {
        p = (PBYTE)val;
    }
    else
    {
        *val = static_cast<ofst>(state->textas.alloc(buffersize));
        p    = (PBYTE)(state->textas.get(*val));
        *p   = 0;
    }
    if (!SetupDiGetDeviceRegistryProperty(hDevInfo, DeviceInfoDataloc, id, &DataT, p, buffersize, &buffersize))
    {
        int ret_er = GetLastError();
        Log.print_file("Property %d\n", id);
        Log.print_syserr(ret_er, L"read_device_property()");
        return;
    }
}

int Device::print_status()
{
    int isPhantom = 0;

    if (ret != CR_SUCCESS)
    {
        if ((ret == CR_NO_SUCH_DEVINST) || (ret == CR_NO_SUCH_VALUE))
            isPhantom = 1;
    }

    if (isPhantom)
        return 0;
    else
    {
        if ((status & DN_HAS_PROBLEM) && problem == CM_PROB_DISABLED)
            return 1;
        else
        {
            if (status & DN_HAS_PROBLEM)
                return 2;
            else if (status & DN_PRIVATE_PROBLEM)
                return 3;
            else if (status & DN_STARTED)
                return 4;
            else
                return 5;
        }
    }
}

void Device::print(const State *state)
{
    static const char *deviceststus_str[] = {
        "Device is not present",
        "Device is disabled",
        "The device has the following problem: %d",
        "The driver reported a problem with the device",
        "Driver is running",
        "Device is currently stopped"};

    const Txt *txt = &state->textas;
    Log.print_file("DeviceInfo\n");
    Log.print_file("  Name:         %S\n", txt->get(Devicedesc));
    Log.print_file("  Status:       ");
    Log.print_file(deviceststus_str[print_status()], problem);
    Log.print_file("\n  Manufacturer: %S\n", txt->getw(Mfg));
    Log.print_file("  HWID_reg      %S\n", txt->getw(Driver));
    Log.print_file("  Class:        ");
    print_guid(&DeviceInfoData.ClassGuid);
    Log.print_file("  Location:     \n");
    Log.print_file("  ConfigFlags:  %d\n", ConfigFlags);
    Log.print_file("  Capabilities: %d\n", Capabilities);
}

void Device::printHWIDS(const State *state)
{
    if (HardwareID)
    {
        const wchar_t *p = state->textas.getw(HardwareID);
        Log.print_file("HardwareID\n");
        while (*p)
        {
            Log.print_file("  %S\n", p);
            p += wcslen(p) + 1;
        }
    }
    else
    {
        Log.print_file("NoID\n");
    }

    if (CompatibleIDs)
    {
        const wchar_t *p = state->textas.getw(CompatibleIDs);
        Log.print_file("CompatibleID\n");
        while (*p)
        {
            Log.print_file("  %S\n", p);
            p += wcslen(p) + 1;
        }
    }
}

void Device::getClassDesc(wchar_t *bufw) { SetupDiGetClassDescription(&DeviceInfoData.ClassGuid, bufw, BUFLEN, nullptr); }

const wchar_t *Device::getHWIDby(int num, const State *state)
{
    int i = 0;

    if (HardwareID)
    {
        const wchar_t *p = state->textas.getw(HardwareID);
        while (*p)
        {
            if (i == num)
                return p;
            p += wcslen(p) + 1;
            i++;
        }
    }
    if (CompatibleIDs)
    {
        const wchar_t *p = state->textas.getw(CompatibleIDs);
        while (*p)
        {
            if (i == num)
                return p;
            p += wcslen(p) + 1;
            i++;
        }
    }
    return L"";
}

Device::Device(State *state)
    : driver_index(-1),
      Devicedesc(0),
      HardwareID(0),
      CompatibleIDs(0),
      Driver(0),
      Mfg(0),
      FriendlyName(0),
      Capabilities(0),
      ConfigFlags(0),
      InstanceId(0),
      status(0),
      problem(0),
      ret(0)
{
    wchar_t buf[BUFLEN];

    // wsprintf(buf,L"%S",ex.GetHWID());
    // Log.print_con("Fake '%S'\n",buf);
    buf[wcslen(buf) + 2] = 0;
    problem              = 2;
    HardwareID           = static_cast<ofst>(state->textas.t_memcpy((char *)buf, wcslen(buf) * 2 + 4));
}

Device::Device(HDEVINFO hDevInfo, State *state, int i)
{
    DWORD buffersize;
    SP_DEVINFO_DATA *DeviceInfoDataloc;

    DeviceInfoDataloc = (SP_DEVINFO_DATA *)&DeviceInfoData;
    memset(&DeviceInfoData, 0, sizeof(SP_DEVINFO_DATA));
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    driver_index = -1;
    if (!SetupDiEnumDeviceInfo(hDevInfo, i, DeviceInfoDataloc))
    {
        ret = GetLastError();
        return;
    }

    SetupDiGetDeviceInstanceId(hDevInfo, DeviceInfoDataloc, nullptr, 0, &buffersize);
    InstanceId = static_cast<ofst>(state->textas.alloc(buffersize));
    SetupDiGetDeviceInstanceId(hDevInfo, DeviceInfoDataloc, const_cast<wchar_t *>(state->textas.getw(InstanceId)), buffersize, nullptr);

    read_device_property(hDevInfo, state, SPDRP_DEVICEDESC, &Devicedesc);
    read_device_property(hDevInfo, state, SPDRP_HARDWAREID, &HardwareID);
    read_device_property(hDevInfo, state, SPDRP_COMPATIBLEIDS, &CompatibleIDs);
    read_device_property(hDevInfo, state, SPDRP_DRIVER, &Driver);
    read_device_property(hDevInfo, state, SPDRP_MFG, &Mfg);
    read_device_property(hDevInfo, state, SPDRP_FRIENDLYNAME, &FriendlyName);
    read_device_property(hDevInfo, state, SPDRP_CAPABILITIES, &Capabilities);
    read_device_property(hDevInfo, state, SPDRP_CONFIGFLAGS, &ConfigFlags);

    ret = CM_Get_DevNode_Status(&status, &problem, DeviceInfoDataloc->DevInst, 0);
    if (ret != CR_SUCCESS)
    {
        Log.print_err("ERROR %d with CM_Get_DevNode_Status()\n", ret);
    }
}
