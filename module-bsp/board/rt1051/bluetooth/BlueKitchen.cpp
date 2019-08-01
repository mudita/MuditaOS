#include "bluetooth/Bluetooth.hpp"
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


ssize_t BlueKitchen::read(void *buf, size_t nbytes)
{
    read_buff = reinterpret_cast<char*>(buf);
    to_read = nbytes;
    // set bt ptr to 0, len to 0, to read to nbytes
    // bt->to_read
    return 0;
}

extern "C" {
    void LPUART2_IRQHandler(void)
    {
        uint32_t isrReg = LPUART_GetStatusFlags(BSP_BLUETOOTH_UART_BASE);
        static char characterReceived  = 0;

        if(isrReg & kLPUART_RxDataRegFullFlag)
        {
            characterReceived = LPUART_ReadByte(BSP_BLUETOOTH_UART_BASE);
            bsp::BlueKitchen *bt = bsp::BlueKitchen::getInstance();
            if( bt->to_read != 0 ) {
                    *bt->read_buff=characterReceived;
                    ++bt->read_buff;
                if( --bt->to_read == 0 && bt->read_ready_cb != NULL) {
                    bt->read_ready_cb();
                }
            }
        }
        if(isrReg & kLPUART_RxOverrunFlag) {
            // TODO signal it
        }
        LPUART_ClearStatusFlags(BSP_BLUETOOTH_UART_BASE, isrReg);
    }
};
