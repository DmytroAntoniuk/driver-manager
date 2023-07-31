#pragma once
#include "device.h"

class State
{
public:
    static State& GetInstance();

    void AddDevice(const Device device);

private:
    State();

    State(const State&)            = delete;
    State& operator=(const State&) = delete;

    Devices _devices;
};
