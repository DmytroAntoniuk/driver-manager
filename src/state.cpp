#include "pch.h"

#include "state.h"

State::State() {}

State& State::GetInstance()
{
    static State instance;
    return instance;
}

void State::AddDevice(const Device device)
{
    LOG_DEBUG("add device");
    _devices.push_back(device);
}
