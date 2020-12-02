// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/bluetooth/Bluetooth.hpp"
#include "BluetoothWorker.hpp"
#include "log/log.hpp"
#include "FreeRTOS.h"
#include "fsl_lpuart.h"
#include "board.h"

#if DEBUG_BLUETOOTH_HCI_COMS >= 1
#define log_hci_stack(...) LOG_DEBUG(__VA_ARGS__)
#else
#define log_hci_stack(...)
#endif

using namespace bsp;

BlueKitchen::BlueKitchen() : BluetoothCommon()
{
    to_read       = 0;
    read_buff     = NULL;
    read_ready_cb = NULL;
    write_done_cb = NULL;
}

BlueKitchen::~BlueKitchen()
{}

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

ssize_t BlueKitchen::read(uint8_t *buf, size_t nbytes)
{
    log_hci_stack("BlueKitchen requested to read %d bytes", nbytes);

    ssize_t i = 0;
    uint8_t val;

    to_read = nbytes;

    read_buff = reinterpret_cast<volatile char *>(buf);
    read_len  = nbytes;

    if (BluetoothCommon::read(buf, nbytes) == nbytes) {
        val = Bt::Message::EvtReceiving;
        xQueueSend(qHandle, &val, portMAX_DELAY);
    }
    else {
        val = Bt::Message::EvtReceivingError;
        xQueueSend(qHandle, &val, portMAX_DELAY);
    }

    return i;
}

void BlueKitchen::set_flowcontrol(int on)
{
    // TODO
}

#include <sstream>

ssize_t BlueKitchen::write(const uint8_t *buf, size_t size)
{

    ssize_t i            = 0;
    uint8_t val;

    log_hci_stack("BlueKitchen sends %d bytes", size);

    if (BluetoothCommon::write(buf, size) == size) {
        val = Bt::Message::EvtSending;
        xQueueSend(qHandle, &val, portMAX_DELAY);
    }
    else {
        val = Bt::Message::EvtSendingError;
        xQueueSend(qHandle, &val, portMAX_DELAY);
    }

    return i;
}
