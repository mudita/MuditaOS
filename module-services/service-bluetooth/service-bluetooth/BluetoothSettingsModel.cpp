// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothSettingsModel.hpp"

#include <service-bluetooth/Constants.hpp>
#include <service-bluetooth/messages/BondedDevices.hpp>
#include <service-bluetooth/messages/Connect.hpp>
#include <service-bluetooth/messages/DeviceName.hpp>
#include <service-bluetooth/messages/Disconnect.hpp>
#include <service-bluetooth/messages/Status.hpp>
#include <service-bluetooth/messages/SetStatus.hpp>
#include <service-bluetooth/messages/SetDeviceName.hpp>
#include <service-bluetooth/messages/Passkey.hpp>
#include <service-bluetooth/messages/Unpair.hpp>

BluetoothSettingsModel::BluetoothSettingsModel(app::Application *application) : application{application}
{}

void BluetoothSettingsModel::requestStatus()
{
    application->bus.sendUnicast(std::make_shared<::message::bluetooth::RequestStatus>(), service::name::bluetooth);
}

void BluetoothSettingsModel::setStatus(const bool desiredBluetoothState, const bool desiredVisibility)
{
    BluetoothStatus status{.state = desiredBluetoothState ? BluetoothStatus::State::On : BluetoothStatus::State::Off,
                           .visibility = desiredVisibility};
    message::bluetooth::SetStatus setStatus(status);
    application->bus.sendUnicast(std::make_shared<::message::bluetooth::SetStatus>(std::move(setStatus)),
                                 service::name::bluetooth);
}

void BluetoothSettingsModel::requestDeviceName()
{
    application->bus.sendUnicast(std::make_shared<::message::bluetooth::RequestDeviceName>(), service::name::bluetooth);
}

void BluetoothSettingsModel::setDeviceName(const UTF8 &deviceName)
{
    application->bus.sendUnicast(std::make_shared<message::bluetooth::SetDeviceName>(deviceName),
                                 service::name::bluetooth);
}

void BluetoothSettingsModel::requestBondedDevices()
{
    application->bus.sendUnicast(std::make_shared<::message::bluetooth::RequestBondedDevices>(),
                                 service::name::bluetooth);
}

void BluetoothSettingsModel::requestScan()
{
    application->bus.sendUnicast(std::make_shared<BluetoothMessage>(BluetoothMessage::Request::Scan),
                                 service::name::bluetooth);
}

void BluetoothSettingsModel::stopScan()
{
    application->bus.sendUnicast(std::make_shared<BluetoothMessage>(BluetoothMessage::Request::StopScan),
                                 service::name::bluetooth);
}

void BluetoothSettingsModel::requestDevicePair(const std::string &addr)
{
    application->bus.sendUnicast(std::make_shared<BluetoothPairMessage>(addr), service::name::bluetooth);
}

void BluetoothSettingsModel::requestDeviceUnpair(const Devicei &device)
{
    application->bus.sendUnicast(std::make_shared<message::bluetooth::Unpair>(bd_addr_to_str(device.address)),
                                 service::name::bluetooth);
}

void BluetoothSettingsModel::responsePasskey(const std::string &passkey)
{
    application->bus.sendUnicast(std::make_shared<message::bluetooth::ResponsePasskey>(passkey),
                                 service::name::bluetooth);
}

void BluetoothSettingsModel::requestConnection(const std::string &addr)
{
    application->bus.sendUnicast(std::make_shared<message::bluetooth::Connect>(addr), service::name::bluetooth);
}

void BluetoothSettingsModel::requestDisconnection()
{
    application->bus.sendUnicast(std::make_shared<message::bluetooth::Disconnect>(), service::name::bluetooth);
}
void BluetoothSettingsModel::mergeDevicesList(const std::vector<Devicei> &devicesList)
{
    devices.insert(std::end(devices), std::begin(devicesList), std::end(devicesList));

    // remove duplicates
    auto end = std::end(devices);
    for (auto it = std::begin(devices); it != end; ++it) {
        end = std::remove(it + 1, end, *it);
    }
    devices.erase(end, std::end(devices));
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
        LOG_FATAL("NO DEVICE FOUND!");
        return std::nullopt;
    }
}
auto BluetoothSettingsModel::getSelectedDevice() -> std::optional<std::reference_wrapper<Devicei>>
{
    try {
        return devices.at(selectedDeviceIndex);
    }
    catch (const std::out_of_range &oor) {
        LOG_FATAL("NO DEVICE FOUND!");
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
void BluetoothSettingsModel::insertDevice(const Devicei device)
{
    devices.emplace_back(device);
}
auto BluetoothSettingsModel::getDeviceByAddress(const std::string &address)
    -> std::optional<std::reference_wrapper<Devicei>>
{
    auto deviceIt = std::find_if(std::begin(devices), std::end(devices), [address](const Devicei &device) {
        return bd_addr_to_str(device.address) == address;
    });

    if (deviceIt == std::end(devices)) {
        return std::nullopt;
    }
    return std::ref(*deviceIt);
}
void BluetoothSettingsModel::removeDevice(const Devicei &device)
{
    devices.erase(std::remove(std::begin(devices), std::end(devices), device), std::end(devices));
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
