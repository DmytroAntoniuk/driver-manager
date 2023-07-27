#pragma once

class Device
{
    int driver_index;

    ofst Devicedesc;
    ofst HardwareID;
    ofst CompatibleIDs;
    ofst Driver;
    ofst Mfg;
    ofst FriendlyName;
    unsigned Capabilities;
    unsigned ConfigFlags;

    ofst InstanceId;
    ULONG status, problem;
    int ret;

    SP_DEVINFO_DATA_32 DeviceInfoData;  // ClassGuid,DevInst

private:
    void print_guid(const GUID *g);
    void read_device_property(HDEVINFO hDevInfo, State *state, int id, ofst *val);

public:
    void setDriverIndex(int v) { driver_index = v; }
    int getDriverIndex() const { return driver_index; }
    ofst getHardwareID() const { return HardwareID; }
    ofst getCompatibleIDs() const { return CompatibleIDs; }
    ofst getFriendlyName() const { return FriendlyName; }
    ofst getDriver() const { return Driver; }
    ofst getDescr() const { return Devicedesc; }
    ofst getMfg() const { return Mfg; }
    ofst getRet() const { return ret; }
    ofst getProblem() const { return problem; }

    int print_status();
    void print(const State *state);
    void printHWIDS(const State *state);
    const wchar_t *getHWIDby(int num, const State *state);
    void getClassDesc(wchar_t *str);

    // Device(const Device &)=delete;
    // Device &operator=(const Device &)=delete;
    // Device(Device &&)=default;
    Device(HDEVINFO hDevInfo, State *state, int i);
    Device(State *state);
    Device()
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
          ret(0),
          DeviceInfoData()
    {
    }

    friend class Manager;  // TODO: friend
    friend class itembar_t;  // TODO: friend
    friend class CanvasImp;
};

loadable_vector<Device> Devices_list;
