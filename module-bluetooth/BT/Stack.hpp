#pragma once


// Uncean of library :( - shall inlcude depending on library selected
#include <string>

extern "C" {
    struct _tagBD_ADDR_t;
    typedef _tagBD_ADDR_t BD_ADDR_t;
}

struct BtDev
{
    BD_ADDR_t *val;
    BtDev();
    const std::string tostr();
    BD_ADDR_t fromstr(char* addr);
    BtDev& operator=(BD_ADDR_t addr);
};

struct Stack {
    int id;
    Stack() : id(0) {}
};
