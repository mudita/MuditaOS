#include "bluetooth/Bluetooth.hpp"
#include "module-bluetooth/Bluetooth/BluetoothWorker.hpp"
#include "log/log.hpp"
#include "FreeRTOS.h"
#include "fsl_lpuart.h"
#include "board.h"

using namespace bsp;

BlueKitchen::BlueKitchen(unsigned int in_size, unsigned int out_size) :BluetoothCommon(in_size,out_size)
{
    to_read=0;
    read_buff = NULL;
    read_ready_cb=NULL;
    write_done_cb=NULL;
}

BlueKitchen::~BlueKitchen()
{
}

BlueKitchen *BlueKitchen::getInstance()
{
    static BlueKitchen *k=NULL;
    if(k==NULL) {
        k=new BlueKitchen();
    }
    return k;
}


// request... from circ buffer
ssize_t BlueKitchen::read(void *buf, size_t nbytes)
{
    to_read = nbytes;
    read_buff = reinterpret_cast<char*>(buf);
    to_read_debug = to_read;
    // set bt ptr to 0, len to 0, to read to nbytes
    // bt->to_read
    return 0;
}

#include <sstream>

ssize_t BlueKitchen::write_blocking(char *buf, ssize_t size) {
    ssize_t i = 0;
    BaseType_t taskwoken = 0;
    uint8_t val = Bt::Message::EvtSent;
    std::stringstream ss;
    for (int i = 0; i < size; ++i) {
        ss << " 0x" << std::hex << (int)buf[i];
    }
    LOG_DEBUG("--> [%d]>%s<", size, ss.str().c_str());
    if (BluetoothCommon::write_blocking(buf, size) == size) {
        xQueueSendFromISR(qHandle, &val, &taskwoken);
    } else {
        val = Bt::Message::EvtSentError;
        xQueueSendFromISR(qHandle, &val, &taskwoken);
    }

    return i;
}

extern "C" {
    // TODO chyba wiem... powinienem zebrac sam dane a potem je przekazac juz zbuforowane xdddd
    void LPUART2_IRQHandler(void)
    {
        uint32_t isrReg = LPUART_GetStatusFlags(BSP_BLUETOOTH_UART_BASE);
        static char characterReceived  = 0;
        BaseType_t taskwoken = 0;
        uint8_t val = Bt::Message::EvtReceived;
        bsp::BlueKitchen *bt = bsp::BlueKitchen::getInstance();

        if(isrReg & kLPUART_RxDataRegFullFlag)
        {
            characterReceived = LPUART_ReadByte(BSP_BLUETOOTH_UART_BASE);
            if( bt->to_read != 0 ) {
                    *(bt->read_buff)=characterReceived;
                    ++bt->read_buff;
                if(--bt->to_read == 0) {
                    BaseType_t taskwoken = 0;
                    uint8_t val = Bt::Message::EvtReceived;
                    if(bt->qHandle) {
                        xQueueSendFromISR(bt->qHandle, &val, &taskwoken);
                        /// tell me about it
                    } else {
                        val = Bt::Message::EvtRecError;
                        xQueueSendFromISR(bt->qHandle, &val, &taskwoken);
                    }
                }
            } else {
                if(bt->qHandle) {
                    val = Bt::Message::EvtRecUnwanted;
                    xQueueSendFromISR(bt->qHandle, &val, &taskwoken);
                }
            }
        }
        if(isrReg & kLPUART_RxOverrunFlag) {
            val = Bt::Message::EvtUartError;
            xQueueSendFromISR(bt->qHandle, &val, &taskwoken);
        }
        LPUART_ClearStatusFlags(BSP_BLUETOOTH_UART_BASE, isrReg);
    }
};
