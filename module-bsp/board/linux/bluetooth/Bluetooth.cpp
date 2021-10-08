// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/bluetooth/Bluetooth.hpp"
#include <log/log.hpp>

/// stubs

using namespace bsp;

BlueKitchen::BlueKitchen() = default;

BlueKitchen *BlueKitchen::getInstance()
{
    static BlueKitchen *k = NULL;
    if (k == NULL) {
        k = new BlueKitchen();
    }
    return k;
}

BluetoothCommon::BluetoothCommon() = default;

BlueKitchen::~BlueKitchen()
{}

BluetoothCommon::~BluetoothCommon()
{}

void BluetoothCommon::open()
{}

void BluetoothCommon::close()
{}

void BluetoothCommon::sleep_ms(ssize_t ms)
{
    ulTaskNotifyTake(pdTRUE, ms);
}

BTdev::Error BlueKitchen::read(uint8_t *buf, size_t nbytes)
{
    return Success;
}

BTdev::Error BluetoothCommon::read(uint8_t *buf, size_t nbytes)
{
    return Success;
}

BTdev::Error BlueKitchen::write(const uint8_t *buf, size_t nbytes)
{
    return Success;
}

BTdev::Error BluetoothCommon::write(const uint8_t *buf, size_t nbytes)
{
    return Success;
}

ssize_t BluetoothCommon::write_blocking(const uint8_t *buf, ssize_t nbytes)
{
    return 0;
}

BTdev::Error BluetoothCommon::set_baudrate(uint32_t bd)
{
    return Success;
}

BTdev::Error BluetoothCommon::set_reset(bool on)
{
    return Success;
}

void BluetoothCommon::init_uart()
{}

void BluetoothCommon::set_irq(bool enable)
{}
