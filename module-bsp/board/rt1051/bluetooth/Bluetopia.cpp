// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bluetooth/Bluetooth.hpp"
#include <log.hpp>
#include "fsl_lpuart.h"
#include "board.h"

namespace bsp
{

    Bluetopia::Bluetopia(unsigned int in_size, unsigned int out_size, int threshold)
        : BluetoothCommon(default_buff_size, default_buff_size, 32)
    {}

    Bluetopia::~Bluetopia()
    {}

    Bluetopia *Bluetopia::getInstance()
    {
        static Bluetopia *inst = NULL;
        if (inst == NULL) {
            inst = new Bluetopia();
        }
        return inst;
    }

    ssize_t Bluetopia::read(void *, size_t nbytes)
    {
        LOG_INFO("not implemented");
        return 0;
    }

    extern "C"
    {
        void LPUART2_IRQHandler(void)
        {
            uint32_t isrReg               = LPUART_GetStatusFlags(BSP_BLUETOOTH_UART_BASE);
            static char characterReceived = 0;

            if (isrReg & kLPUART_RxDataRegFullFlag) {
                characterReceived  = LPUART_ReadByte(BSP_BLUETOOTH_UART_BASE);
                bsp::Bluetopia *bt = bsp::Bluetopia::getInstance();
                if (bt->in.push(characterReceived) == 0) {
                    bt->set_data();
                }
                if (bt->in.threshold_guard()) {
                    bt->set_rts(false);
                }
            }
            // TODO ths should be handled - othervise uart might be `nicelly` blocked
            if (isrReg & kLPUART_RxOverrunFlag) {
                printf("Overrun\n");
            }
            LPUART_ClearStatusFlags(BSP_BLUETOOTH_UART_BASE, isrReg);
        }
    };
