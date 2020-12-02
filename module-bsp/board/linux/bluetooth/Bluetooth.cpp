// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/bluetooth/Bluetooth.hpp"
#include "BluetoothWorker.hpp"
#include "log/log.hpp"

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

ssize_t BlueKitchen::write(const uint8_t *buf, size_t nbytes)
{
    return 0;
}

BluetoothCommon::BluetoothCommon() : BTdev()
{}

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

ssize_t BluetoothCommon::write(char *buf, size_t nbytes)
{
    return 0;
}

ssize_t BluetoothCommon::write_blocking(char *buf, ssize_t len)
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

void BluetoothCommon::configure_uart_io()
{}

void BluetoothCommon::configure_lpuart()
{}

void BluetoothCommon::configure_cts_irq()
{}

void BluetoothCommon::set_irq(bool enable)
{}

ssize_t BlueKitchen::read(void *buf, size_t nbytes)
{
    return 0;
}

