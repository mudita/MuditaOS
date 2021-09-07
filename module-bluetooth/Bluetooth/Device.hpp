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

namespace TYPE_OF_SERVICE
{
    inline constexpr uint32_t POSITIONING     = 0x00010000;
    inline constexpr uint32_t NETWORKING      = 0x00020000;
    inline constexpr uint32_t RENDERING       = 0x00040000;
    inline constexpr uint32_t CAPTURING       = 0x00080000;
    inline constexpr uint32_t OBJECT_TRANSFER = 0x00100000;
    inline constexpr uint32_t AUDIO           = 0x00200000;
    inline constexpr uint32_t TELEPHONY       = 0x00400000;
    inline constexpr uint32_t INFORMATION     = 0x00800000;

    ///> At least one of this class has to be supported by remote device in order to establish connection
    inline constexpr uint32_t REMOTE_SUPPORTED_SERVICES = (AUDIO | POSITIONING | RENDERING);

} // namespace TYPE_OF_SERVICE

static inline std::string getListOfSupportedServicesInString(uint32_t cod)
{
    std::string res = "|";
    if (cod & TYPE_OF_SERVICE::POSITIONING) {
        res += "POSITIONING|";
    }
    if (cod & TYPE_OF_SERVICE::NETWORKING) {
        res += "NETWORKING|";
    }
    if (cod & TYPE_OF_SERVICE::RENDERING) {
        res += "RENDERING|";
    }
    if (cod & TYPE_OF_SERVICE::CAPTURING) {
        res += "CAPTURING|";
    }
    if (cod & TYPE_OF_SERVICE::OBJECT_TRANSFER) {
        res += "OBJECT_TRANSFER|";
    }
    if (cod & TYPE_OF_SERVICE::AUDIO) {
        res += "AUDIO|";
    }
    if (cod & TYPE_OF_SERVICE::TELEPHONY) {
        res += "TELEPHONY|";
    }
    if (cod & TYPE_OF_SERVICE::INFORMATION) {
        res += "INFORMATION|";
    }
    if (res == std::string("|")) {
        res += "NONE|";
    }
    return res;
}

struct Devicei : public Device
{
  public:
    bd_addr_t address;
    uint8_t pageScanRepetitionMode;
    uint16_t clockOffset;
    uint32_t classOfDevice; ///> Class of Device/Service
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
