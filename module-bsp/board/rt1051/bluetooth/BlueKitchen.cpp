// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "bsp/bluetooth/Bluetooth.hpp"
#include <log/log.hpp>
#include "FreeRTOS.h"

#if DEBUG_BLUETOOTH_HCI_COMS == 1
#define logHciStack(...) LOG_DEBUG(__VA_ARGS__)
#else
#define logHciStack(...)
#endif

using namespace bsp;

BlueKitchen::BlueKitchen()
{
    readBuffer        = nullptr;
    readReadyCallback = nullptr;
    writeDoneCallback = nullptr;
}

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
    logHciStack("BlueKitchen requested to read %d bytes", size);

    std::uint8_t val;

    readBuffer = buf;
    readLength = size;

    if (BluetoothCommon::read(buf, size) == BTDevice::Error::Success) {
        val = bluetooth::Message::EvtReceiving;
        xQueueSend(qHandle, &val, portMAX_DELAY);
        return BTDevice::Error::Success;
    }
    else {
        val = bluetooth::Message::EvtReceivingError;
        xQueueSend(qHandle, &val, portMAX_DELAY);
        return BTDevice::Error::ErrorBSP;
    }
}

BTDevice::Error BlueKitchen::write(const std::uint8_t *buf, std::size_t size)
{
    logHciStack("BlueKitchen sends %d bytes", size);

    std::uint8_t val;

    if (BluetoothCommon::write(buf, size) == BTDevice::Error::Success) {
        val = bluetooth::Message::EvtSending;
        xQueueSend(qHandle, &val, portMAX_DELAY);
        return BTDevice::Error::Success;
    }
    else {
        val = bluetooth::Message::EvtSendingError;
        xQueueSend(qHandle, &val, portMAX_DELAY);
        return BTDevice::Error::ErrorBSP;
    }
}
