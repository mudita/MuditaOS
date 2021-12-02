// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Device.hpp"
#include <btstack_util.h>

Devicei::Devicei(std::string name) : Device(std::move(name))
{
    pageScanRepetitionMode = {};
    clockOffset            = {};
    classOfDevice          = {};
    state                  = DEVICE_STATE::REMOTE_NAME_FAILURE;
    deviceState            = DeviceState::Unknown;
    isPairingSSP           = false;

    memset(address, 0, sizeof(address));
}

Devicei::Devicei(bd_addr_t &address) : Devicei()
{
    setAddress(&address);
}

Devicei &Devicei::operator=(const Devicei &d)
{
    if (&d == this) {
        return *this;
    }
    setAddress(&d.address);
    setName(d.name.data());
    pageScanRepetitionMode = d.pageScanRepetitionMode;
    clockOffset            = d.clockOffset;
    classOfDevice          = d.classOfDevice;
    state                  = d.state;
    deviceState            = d.deviceState;
    isPairingSSP           = d.isPairingSSP;
    return *this;
}

Devicei::Devicei(const Devicei &d) : Devicei(d.name.data())
{
    operator=(d);
}

Devicei &Devicei::operator=(Devicei &&d) noexcept
{
    setAddress(&d.address);
    setName(d.name.data());
    pageScanRepetitionMode = d.pageScanRepetitionMode;
    clockOffset            = d.clockOffset;
    classOfDevice          = d.classOfDevice;
    state                  = d.state;
    deviceState            = d.deviceState;
    isPairingSSP           = d.isPairingSSP;
    return *this;
}

Devicei::Devicei(Devicei &&d) noexcept : Devicei(d.name.data())
{
    operator=(d);
}

void Devicei::setAddress(bd_addr_t *addr)
{
    memcpy(&address, addr, sizeof address);
}

bool Devicei::operator==(const Devicei &cmpDevice) const
{
    return (strcmp(cmpDevice.name.data(), name.data()) == 0) && (bd_addr_cmp(cmpDevice.address, address) == 0);
}

bool Devicei::operator!=(const Devicei &cmpDevice) const
{
    return (strcmp(cmpDevice.name.data(), name.data()) != 0) || (bd_addr_cmp(cmpDevice.address, address) != 0);
}

auto Devicei::address_str() const -> const char *
{
    return bd_addr_to_str(address);
}
