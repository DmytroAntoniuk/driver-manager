#pragma once

#include "driverpack.h"

class driverpack_task
{
    Driverpack *drp;
    driverpack_task(Driverpack *a) : drp(a) {}
    driverpack_task() : drp(nullptr) {}

    friend class Driverpack;
    friend class Collection;
};
