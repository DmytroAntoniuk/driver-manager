#pragma once

class Device
{
    Device(const HDEVINFO h_dev_info, const DWORD member_index);

private:
    unsigned _instance_id;
};
