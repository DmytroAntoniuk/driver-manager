#pragma once
#include "device.h"

class State
{
public:
    static State& GetInstance();

    Devices GetDevices() const;
    Device AddDevice(HDEVINFO h_dev_info, SP_DEVINFO_DATA& dev_info_data);

private:
    State();

    State(const State&)            = delete;
    State& operator=(const State&) = delete;

    Devices _devices;
};
