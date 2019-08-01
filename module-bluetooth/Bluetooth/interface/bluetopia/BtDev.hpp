#pragma once


// Uncean of library :( - shall inlcude depending on library selected
#include <string>
#include <Bluetooth/Device.hpp>

extern "C" {
    struct _tagBD_ADDR_t;
    typedef _tagBD_ADDR_t BD_ADDR_t;
}

struct BtDev : public Device
{
    BD_ADDR_t *val;
    BtDev();
    ~BtDev();
    const std::string tostr();
    BD_ADDR_t fromstr(char* addr);
    BtDev& operator=(BD_ADDR_t addr);
};
