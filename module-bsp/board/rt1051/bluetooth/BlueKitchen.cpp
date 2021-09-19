// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/bluetooth/Bluetooth.hpp"
#include <log.hpp>
#include "FreeRTOS.h"
#include "fsl_lpuart.h"
#include <board/board.h>

#if DEBUG_BLUETOOTH_HCI_COMS == 1
#define logHciStack(...) LOG_DEBUG(__VA_ARGS__)
#else
#define logHciStack(...)
#endif

using namespace bsp;

BlueKitchen::BlueKitchen()
{
    read_buff     = NULL;
    read_ready_cb = NULL;
    write_done_cb = NULL;
}

BlueKitchen::~BlueKitchen() = default;

BlueKitchen *BlueKitchen::getInstance()
{
    static BlueKitchen *k = NULL;
    if (k == NULL) {
        /// outcomming & incomming heap allocated buffers sizes
        /// packet on IP network cna have MTU 1500, so big enough buffers were added to not throttle comms
        k = new BlueKitchen();
    }
    return k;
}

BTdev::Error BlueKitchen::read(uint8_t *buf, size_t nbytes)
{
    logHciStack("BlueKitchen requested to read %d bytes", nbytes);

    uint8_t val;

    read_buff = buf; // point at the Bt stack read buffer
    read_len  = nbytes;

    if (BluetoothCommon::read(buf, nbytes) == Success) {
        val = bluetooth::Message::EvtReceiving;
        xQueueSend(qHandle, &val, portMAX_DELAY);
        return BTdev::Success;
    }
    else {
        val = bluetooth::Message::EvtReceivingError;
        xQueueSend(qHandle, &val, portMAX_DELAY);
        return BTdev::ErrorBSP;
    }
}

BTdev::Error BlueKitchen::write(const uint8_t *buf, size_t size)
{
    uint8_t val;

    logHciStack("BlueKitchen sends %d bytes", size);

    if (BluetoothCommon::write(buf, size) == Success) {
        val = bluetooth::Message::EvtSending;
        xQueueSend(qHandle, &val, portMAX_DELAY);
        return BTdev::Success;
    }
    else {
        val = bluetooth::Message::EvtSendingError;
        xQueueSend(qHandle, &val, portMAX_DELAY);
        return BTdev::ErrorBSP;
    }
}

void BlueKitchen::set_flowcontrol(int on)
{}
