// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothDevicesModel.hpp"
#include <Service/Service.hpp>
#include <service-bluetooth/messages/BondedDevices.hpp>
#include <log/log.hpp>

BluetoothDevicesModel::BluetoothDevicesModel(sys::Service *service) : service{service}
{}

void BluetoothDevicesModel::removeDeviceDuplicates()
{
    auto end = std::end(devices);
    for (auto it = std::begin(devices); it != end; ++it) {
        end = std::remove(it + 1, end, *it);
    }
    devices.erase(end, std::end(devices));
}

void BluetoothDevicesModel::mergeDevicesList(const Devicei &device)
{
    devices.emplace_back(device);
    removeDeviceDuplicates();
}

void BluetoothDevicesModel::mergeDevicesList(const std::vector<Devicei> &devicesList)
{
    devices.insert(std::end(devices), std::begin(devicesList), std::end(devicesList));
    removeDeviceDuplicates();
}

void BluetoothDevicesModel::insertDevice(const Devicei &device)
{
    devices.emplace_back(device);
    syncDevicesWithApp();
}
auto BluetoothDevicesModel::getDeviceByAddress(const std::string &address)
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
auto BluetoothDevicesModel::getDeviceByAddress(const bd_addr_t address)
    -> std::optional<std::reference_wrapper<Devicei>>
{

    auto deviceIt = std::find_if(std::begin(devices), std::end(devices), [address](const Devicei &device) {
        return std::memcmp(address, device.address, sizeof(bd_addr_t)) == 0;
    });

    if (deviceIt == std::end(devices)) {
        return std::nullopt;
    }
    return std::ref(*deviceIt);
}
void BluetoothDevicesModel::removeDevice(const Devicei &device)
{
    auto position = std::find(std::begin(devices), std::end(devices), device);
    if (position != std::end(devices)) {
        devices.erase(position);
    }
    syncDevicesWithApp();
}
auto BluetoothDevicesModel::getDevices() -> std::vector<Devicei> &
{
    return devices;
}
void BluetoothDevicesModel::syncDevicesWithApp()
{
    if (service != nullptr) {
        service->bus.sendMulticast(std::make_shared<::message::bluetooth::SyncDevices>(devices),
                                   sys::BusChannel::BluetoothNotifications);
    }
}

void BluetoothDevicesModel::setInternalDeviceState(const Devicei &device, const DeviceState &state)
{
    auto dev                      = getDeviceByAddress(device.address);
    if (not dev) {
        LOG_ERROR("no such device - ignored");
        return;
    }
    dev.value().get().deviceState = state;
}

void BluetoothDevicesModel::mergeInternalDeviceState(const Devicei &device)
{
    try {
        auto deviceInModel = getDeviceByAddress(device.address).value().get();

        if ((deviceInModel.deviceState == DeviceState::ConnectedVoice &&
             device.deviceState == DeviceState::ConnectedAudio) ||
            (device.deviceState == DeviceState::ConnectedVoice &&
             deviceInModel.deviceState == DeviceState::ConnectedAudio)) {
            setInternalDeviceState(device, DeviceState::ConnectedBoth);
        }
        else {
            setInternalDeviceState(device, device.deviceState);
        }
    }
    catch (std::bad_optional_access &e) {
        LOG_ERROR("Can't get device by address! Unable to update device state. %s", e.what());
        setInternalDeviceState(device, device.deviceState);
    }
}
