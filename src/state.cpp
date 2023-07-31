#include "pch.h"

#include "state.h"

State::State() {}

State& State::GetInstance()
{
    static State instance;
    return instance;
}

Devices State::GetDevices() const { return _devices; }

Device State::AddDevice(HDEVINFO h_dev_info, SP_DEVINFO_DATA& dev_info_data)
{
    LOG_DEBUG("add device");
    return _devices.emplace_back(h_dev_info, dev_info_data);
}
