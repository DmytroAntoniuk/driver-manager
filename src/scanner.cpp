#include "pch.h"

#include "scanner.h"

#include <fstream>
#include <logger/logger.h>

#define BUFLEN 4096

void Scanner::Run()
{
    LOG_DEBUG("scanner run");
    PVOID h_dev_info;
    HKEY h_key;
    wchar_t buf[BUFLEN];

    h_dev_info = SetupDiGetClassDevs(nullptr, nullptr, nullptr, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (h_dev_info == INVALID_HANDLE_VALUE)
    {
        LOG_ERROR("INVALID_HANDLE_VALUE");
        return;
    }
}
