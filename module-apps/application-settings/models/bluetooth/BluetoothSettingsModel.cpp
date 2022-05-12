// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothSettingsModel.hpp"

#include <service-bluetooth/Constants.hpp>
#include <service-bluetooth/messages/BondedDevices.hpp>
#include <service-bluetooth/messages/Connect.hpp>
#include <service-bluetooth/messages/DeviceName.hpp>
#include <service-bluetooth/messages/Disconnect.hpp>
#include <service-bluetooth/messages/SetStatus.hpp>
#include <service-bluetooth/messages/SetDeviceName.hpp>
#include <service-bluetooth/messages/Authenticate.hpp>
#include <service-bluetooth/messages/Unpair.hpp>
#include <service-bluetooth/messages/SyncDevices.hpp>

BluetoothSettingsModel::BluetoothSettingsModel(sys::Service *service) : service{service}
{}

void BluetoothSettingsModel::requestStatus()
{
    service->bus.sendUnicast(std::make_shared<::message::bluetooth::RequestStatus>(), service::name::bluetooth);
}

void BluetoothSettingsModel::setStatus(const bool desiredBluetoothState, const bool desiredVisibility)
{
    status.state      = desiredBluetoothState ? BluetoothStatus::State::On : BluetoothStatus::State::Off;
    status.visibility = desiredVisibility;
    message::bluetooth::SetStatus setStatus(status);
    service->bus.sendUnicast(std::make_shared<::message::bluetooth::SetStatus>(std::move(setStatus)),
                             service::name::bluetooth);
}

void BluetoothSettingsModel::requestDeviceName()
{
    service->bus.sendUnicast(std::make_shared<::message::bluetooth::RequestDeviceName>(), service::name::bluetooth);
}

void BluetoothSettingsModel::setDeviceName(const UTF8 &deviceName)
{
    service->bus.sendUnicast(std::make_shared<message::bluetooth::SetDeviceName>(deviceName), service::name::bluetooth);
}

void BluetoothSettingsModel::requestBondedDevices()
{
    service->bus.sendUnicast(std::make_shared<::message::bluetooth::RequestBondedDevices>(), service::name::bluetooth);
}

void BluetoothSettingsModel::requestScan()
{
    /// TODO send event Scan{}
    service->bus.sendUnicast(std::make_shared<BluetoothMessage>(BluetoothMessage::Request::Scan),
                             service::name::bluetooth);
}

void BluetoothSettingsModel::stopScan()
{
    service->bus.sendUnicast(std::make_shared<BluetoothMessage>(BluetoothMessage::Request::StopScan),
                             service::name::bluetooth);
}

void BluetoothSettingsModel::requestDevicePair(const Devicei &device)
{
    service->bus.sendUnicast(std::make_shared<BluetoothPairMessage>(device), service::name::bluetooth);
}

void BluetoothSettingsModel::requestDeviceUnpair(const Devicei &device)
{
    service->bus.sendUnicast(std::make_shared<message::bluetooth::Unpair>(device), service::name::bluetooth);
}

void BluetoothSettingsModel::requestConnection(const Devicei &device)
{
    service->bus.sendUnicast(std::make_shared<message::bluetooth::Connect>(device), service::name::bluetooth);
}

void BluetoothSettingsModel::requestDisconnection()
{
    service->bus.sendUnicast(std::make_shared<message::bluetooth::Disconnect>(), service::name::bluetooth);
}
void BluetoothSettingsModel::replaceDevicesList(const std::vector<Devicei> &devicesList)
{
    devices = devicesList;
}
void BluetoothSettingsModel::setActiveDeviceState(const DeviceState &state)
{
    auto activeDevice = getActiveDevice();
    if (activeDevice.has_value()) {
        activeDevice.value().get().deviceState = state;
    }
}
auto BluetoothSettingsModel::getActiveDevice() -> std::optional<std::reference_wrapper<Devicei>>
{
    try {
        return devices.at(activeDeviceIndex);
    }
    catch (const std::out_of_range &oor) {
        LOG_WARN("NO DEVICE FOUND!");
        return std::nullopt;
    }
}
auto BluetoothSettingsModel::getSelectedDevice() -> std::optional<std::reference_wrapper<Devicei>>
{
    try {
        return devices.at(selectedDeviceIndex);
    }
    catch (const std::out_of_range &oor) {
        LOG_WARN("NO DEVICE FOUND!");
        return std::nullopt;
    }
}
void BluetoothSettingsModel::setActiveDevice(const Devicei &device)
{
    auto itr          = std::find(std::begin(devices), std::end(devices), device);
    activeDeviceIndex = std::distance(std::begin(devices), itr);
}
void BluetoothSettingsModel::setSelectedDevice(const Devicei &device)
{
    auto itr            = std::find(std::begin(devices), std::end(devices), device);
    selectedDeviceIndex = std::distance(std::begin(devices), itr);
}

auto BluetoothSettingsModel::getDevices() -> std::vector<Devicei> &
{
    return devices;
}
auto BluetoothSettingsModel::isDeviceConnecting() -> bool
{

    auto deviceIt = std::find_if(std::begin(devices), std::end(devices), [](const Devicei &device) {
        return device.deviceState == DeviceState::Connecting;
    });

    if (deviceIt != std::end(devices)) {
        return true;
    }
    return false;
}
auto BluetoothSettingsModel::getStatus() const -> const BluetoothStatus
{
    return status;
}
auto BluetoothSettingsModel::isDeviceListEmpty() const -> bool
{
    return devices.empty();
}
