// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include <Device.hpp>
#include <service-bluetooth/messages/Status.hpp>

extern "C"
{
#include <module-bluetooth/lib/btstack/src/btstack_util.h>
}

class BluetoothSettingsModel
{
  public:
    explicit BluetoothSettingsModel(sys::Service *service);

    void requestStatus();
    void setStatus(bool desiredBluetoothState, bool desiredVisibility);
    void requestDeviceName();
    void setDeviceName(const UTF8 &deviceName);
    void requestBondedDevices();
    void requestScan();
    void stopScan();
    void requestDevicePair(const Devicei &device);
    void requestDeviceUnpair(const Devicei &device);
    void responsePasskey(const std::string &passkey);
    void requestConnection(const Devicei &device);
    void requestDisconnection();

    void replaceDevicesList(const std::vector<Devicei> &devicesList);
    void setActiveDeviceState(const DeviceState &state);
    auto getActiveDevice() -> std::optional<std::reference_wrapper<Devicei>>;
    auto getSelectedDevice() -> std::optional<std::reference_wrapper<Devicei>>;
    void setActiveDevice(const Devicei &device);
    void setSelectedDevice(const Devicei &device);
    auto getDevices() -> std::vector<Devicei> &;
    auto isDeviceConnecting() -> bool;
    auto getStatus() const -> const BluetoothStatus;

  private:
    std::vector<Devicei> devices{};
    std::uint16_t activeDeviceIndex   = 0;
    std::uint16_t selectedDeviceIndex = 0;
    sys::Service *service             = nullptr;
    BluetoothStatus status{};
};
