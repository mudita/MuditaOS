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

// request... from circ buffer
ssize_t BlueKitchen::read(void *buf, size_t nbytes)
{
    set_rts(false);
    to_read     = nbytes;
    to_read_req = nbytes;
    read_buff   = reinterpret_cast<char *>(buf);
    // set bt ptr to 0, len to 0, to read to nbytes
    // bt->to_read
    BaseType_t taskwoken = 0;
    uint8_t val          = Bt::Message::EvtReceived;
    if ((to_read != 0) && (in.len >= to_read)) {
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

extern "C"
{
    void LPUART2_IRQHandler(void)
    {
        uint32_t isrReg               = LPUART_GetStatusFlags(BSP_BLUETOOTH_UART_BASE);
        static char characterReceived = 0;
        BaseType_t taskwoken          = 0;
        uint8_t val                   = Bt::Message::EvtReceived;
        bsp::BlueKitchen *bt          = bsp::BlueKitchen::getInstance();

        if (isrReg & kLPUART_RxDataRegFullFlag) {
            characterReceived = LPUART_ReadByte(BSP_BLUETOOTH_UART_BASE);
            if (bt->in.push(characterReceived)) {
                val = Bt::Message::EvtRecUnwanted;
                xQueueSendFromISR(bt->qHandle, &val, &taskwoken);
            }
            if (bt->to_read != 0 && (bt->in.len >= bt->to_read)) {
                bt->to_read = 0;
                assert(bt->qHandle);
                val = Bt::Message::EvtReceived;
                xQueueSendFromISR(bt->qHandle, &val, &taskwoken);
                portEND_SWITCHING_ISR(taskwoken);
            }
            if (bt->in.threshold_guard()) {
                bt->set_rts(false);
            }
        }
        if (isrReg & kLPUART_RxOverrunFlag) {
            val = Bt::Message::EvtUartError;
            xQueueSendFromISR(bt->qHandle, &val, &taskwoken);
        }
        LPUART_ClearStatusFlags(BSP_BLUETOOTH_UART_BASE, isrReg);
    }
};
