// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include <Device.hpp>

extern "C"
{
#include <module-bluetooth/lib/btstack/src/btstack_util.h>
}

class ActiveDevice
{
  public:
    explicit ActiveDevice(std::string address) : address(std::move(address))
    {}
    ActiveDevice()    = default;
    DeviceState state = DeviceState::Unknown;
    std::string address;
};

class BluetoothSettingsModel
{
  public:
    explicit BluetoothSettingsModel(app::Application *application);

    void requestStatus();
    void setStatus(bool desiredBluetoothState, bool desiredVisibility);
    void requestDeviceName();
    void setDeviceName(const UTF8 &deviceName);
    void requestBondedDevices();
    void requestScan();
    void stopScan();
    void requestDevicePair(const std::string &addr);
    void requestDeviceUnpair(const Devicei &device);
    void responsePasskey(const std::string &passkey);
    void requestConnection(const std::string &addr);
    void requestDisconnection();
    void replaceDevicesList(const std::vector<Devicei> &devicesList);
    void setActiveDeviceState(const DeviceState &state);
    auto getActiveDevice() -> std::optional<std::reference_wrapper<Devicei>>;
    auto getSelectedDevice() -> std::optional<std::reference_wrapper<Devicei>>;
    auto getDeviceByAddress(const std::string &address) -> std::optional<std::reference_wrapper<Devicei>>;
    void setActiveDevice(const Devicei &device);
    void setSelectedDevice(const Devicei &device);
    void insertDevice(Devicei device);
    void removeDevice(const Devicei &device);
    auto getDevices() -> std::vector<Devicei> &;
    auto isDeviceConnecting() -> bool;

  private:
    std::vector<Devicei> devices{};
    std::uint16_t activeDeviceIndex   = 0;
    std::uint16_t selectedDeviceIndex = 0;
    app::Application *application     = nullptr;
    Devicei dummyDevice{""};
};
