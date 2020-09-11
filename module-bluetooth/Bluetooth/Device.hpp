#pragma once
#include <string>
#include <cstring>
#include <utility>
#include <module-bluetooth/lib/btstack/src/bluetooth.h>

struct Device
{
  public:
    explicit Device(std::string name = "") : name(std::move(name))
    {}
    virtual ~Device() = default;
    std::string name;
};

enum DEVICE_STATE
{
    REMOTE_NAME_REQUEST,
    REMOTE_NAME_INQUIRED,
    REMOTE_NAME_FETCHED
};

struct Devicei : public Device
{
  public:
    bd_addr_t address;
    uint8_t pageScanRepetitionMode;
    uint16_t clockOffset;
    DEVICE_STATE state;

    Devicei(std::string name = "") : Device(std::move(name))
    {}
    ~Devicei() override = default;
    void address_set(bd_addr_t *addr)
    {
        memcpy(&address, addr, sizeof address);
    }
};
