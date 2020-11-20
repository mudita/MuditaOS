// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/bluetooth/Bluetooth.hpp"
#include "BluetoothWorker.hpp"
#include "log/log.hpp"
#include "FreeRTOS.h"
#include "fsl_lpuart.h"
#include "board.h"

using namespace bsp;

BlueKitchen::BlueKitchen(unsigned int in_size, unsigned int out_size) : BluetoothCommon(in_size, out_size)
{
    to_read       = 0;
    read_buff     = NULL;
    read_ready_cb = NULL;
    write_done_cb = NULL;
    in.threshold  = 128;
}

BlueKitchen::~BlueKitchen()
{}

BlueKitchen *BlueKitchen::getInstance()
{
    static BlueKitchen *k = NULL;
    if (k == NULL) {
        /// outcomming & incomming heap allocated buffers sizes
        /// packet on IP network cna have MTU 1500, so big enough buffers were added to not throttle comms
        k = new BlueKitchen(2048, 8000);
    }
    return k;
}

ssize_t BlueKitchen::read(void *buf, size_t nbytes)
{
    LOG_DEBUG("BlueKitchen requested %d bytes to read", nbytes);
    set_rts(false);
    to_read     = nbytes;
    to_read_req = nbytes;
    read_buff   = reinterpret_cast<char *>(buf);
    // set bt ptr to 0, len to 0, to read to nbytes
    // bt->to_read
    BaseType_t taskwoken = 0;
    uint8_t val          = Bt::Message::EvtReceived;

    if ((to_read != 0) && (in.len >= to_read)) {

        // order our worker to give nbytes RX bytes over to BlueKitchen

        to_read = 0;
        if (qHandle) {
            xQueueSendFromISR(qHandle, &val, &taskwoken);
            portEND_SWITCHING_ISR(taskwoken);
        }
    }
    set_rts(true);
    return 0;
}

void BlueKitchen::set_flowcontrol(int on)
{
    // TODO
}

#include <sstream>

ssize_t BlueKitchen::write_blocking(char *buf, ssize_t size)
{
    ssize_t i            = 0;
    BaseType_t taskwoken = 0;
    uint8_t val          = Bt::Message::EvtSent;

#define DO_DEBUG_HCI_COMS
#ifdef DO_DEBUG_HCI_COMS
    std::stringstream ss;
    for (int i = 0; i < size; ++i) {
        ss << " 0x" << std::hex << (int)buf[i];
    }
    LOG_DEBUG("--> [%d]>%s<", size, ss.str().c_str());
#endif
    if (BluetoothCommon::write_blocking(buf, size) == size) {
        xQueueSendFromISR(qHandle, &val, &taskwoken);
        portEND_SWITCHING_ISR(taskwoken);
    }
    else {
        val = Bt::Message::EvtSentError;
        xQueueSendFromISR(qHandle, &val, &taskwoken);
    }

    return i;
}

;
