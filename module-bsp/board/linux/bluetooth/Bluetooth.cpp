// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/bluetooth/Bluetooth.hpp"
#include <memory>

using namespace bsp;

BlueKitchen::BlueKitchen() = default;
BlueKitchen::~BlueKitchen() = default;

BlueKitchen &BlueKitchen::getInstance()
{
    static std::unique_ptr<BlueKitchen> instance;
    if (instance == nullptr) {
        instance = std::make_unique<BlueKitchen>();
    }
    return *instance;
}

BTDevice::Error BlueKitchen::read(std::uint8_t *buf, std::size_t size)
{
    return BTDevice::Error::Success;
}

BTDevice::Error BlueKitchen::write(const std::uint8_t *buf, std::size_t size)
{
    return BTDevice::Error::Success;
}

BluetoothCommon::BluetoothCommon()  = default;
BluetoothCommon::~BluetoothCommon() = default;

void BluetoothCommon::open()
{}

void BluetoothCommon::close()
{}

void BluetoothCommon::sleepMs(std::size_t ms)
{
    ulTaskNotifyTake(pdTRUE, ms);
}

BTDevice::Error BluetoothCommon::read(std::uint8_t *buf, std::size_t size)
{
    return BTDevice::Error::Success;
}

BTDevice::Error BluetoothCommon::write(const std::uint8_t *buf, std::size_t size)
{
    return BTDevice::Error::Success;
}

BTDevice::Error BluetoothCommon::setBaudrate([[maybe_unused]] std::uint32_t baud)
{
    return BTDevice::Error::Success;
}

BTDevice::Error BluetoothCommon::setReset([[maybe_unused]] bool on)
{
    return BTDevice::Error::Success;
}

void BluetoothCommon::uartInit()
{}

void BluetoothCommon::setIrq([[maybe_unused]] bool enable)
{}
