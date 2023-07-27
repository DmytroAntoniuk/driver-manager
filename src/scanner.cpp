#include "pch.h"

#include "scanner.h"

#include "device.h"

#define BUFLEN 4096

void Scanner::Run()
{
    LOG_DEBUG("scanner run");
    HDEVINFO h_dev_info;
    HKEY h_key;
    wchar_t buf[BUFLEN];

    h_dev_info = SetupDiGetClassDevs(nullptr, nullptr, nullptr, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (h_dev_info == INVALID_HANDLE_VALUE)
    {
        LOG_ERROR("can't get devices list. error: {}", GetLastError());
        return;
    }

    Device device(h_dev_info, 0);
}
