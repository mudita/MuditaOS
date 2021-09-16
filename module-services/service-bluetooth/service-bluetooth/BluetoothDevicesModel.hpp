// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Device.hpp>
#include "service-bluetooth/messages/SyncDevices.hpp"
extern "C"
{
#include <module-bluetooth/lib/btstack/src/btstack_util.h>
}

class BluetoothDevicesModel
{
  public:
    explicit BluetoothDevicesModel(sys::Service *service);

    void mergeDevicesList(const std::vector<Devicei> &devicesList);
    auto getDeviceByAddress(const std::string &address) -> std::optional<std::reference_wrapper<Devicei>>;
    auto getDeviceByAddress(const bd_addr_t address) -> std::optional<std::reference_wrapper<Devicei>>;
    void insertDevice(const Devicei &device);
    void removeDevice(const Devicei &device);
    auto getDevices() -> std::vector<Devicei> &;
    void syncDevicesWithApp();
    void setInternalDeviceState(const Devicei &device, const DeviceState &state);

  private:
    std::vector<Devicei> devices{};
    sys::Service *service = nullptr;
};
