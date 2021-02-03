// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <array>
#include <cstring>
#include <module-bluetooth/lib/btstack/src/bluetooth.h>
#include <string>
#include <utility>

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
    void setAddress(bd_addr_t *addr)
    {
        memcpy(&address, addr, sizeof address);
    }
};

struct DeviceMetadata_t
{
    unsigned int sampleRate;
    unsigned short channels;
    unsigned int samplesPerFrame;
};

constexpr unsigned int DATA_BUFFER_SIZE = 256 * 2;

struct AudioData_t
{
    std::array<int16_t, DATA_BUFFER_SIZE> data;
    uint16_t bytesSent;
};
