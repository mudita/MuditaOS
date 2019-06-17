
/*
 * @file rt1501_cellular.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 17.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_RT1501_CELLULAR_HPP
#define PUREPHONE_RT1501_CELLULAR_HPP

#include "cellular/bsp_cellular.hpp"

#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "fsl_lpuart.h"
#include "fsl_lpuart_edma.h"
#include "fsl_dmamux.h"

namespace bsp {

    class RT1051Cellular : public Cellular {
    public:

        RT1051Cellular();

        ~RT1051Cellular();

        uint32_t Wait(uint32_t timeout) override final;

        uint32_t Read(void *buf, size_t nbytes) override final;

        uint32_t Write(void *buf, size_t nbytes) override final;



        static StreamBufferHandle_t         uartRxStreamBuffer;

    private:
        void MSPInit();
        void DMAInit();
        uint32_t UartGetPeripheralClock();
        void EnableRx();
        void DisableRx();


        static lpuart_edma_handle_t         uartDmaHandle;
        static edma_handle_t                uartTxDmaHandle;

        static void DMATxCompletedCb(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData);




        const static uint32_t baudrate = 115200;

        const static uint32_t rxStreamBufferLength = 1024;
        const static uint32_t rxStreamBufferNotifyWatermark = 1;

    };

}


#endif //PUREPHONE_RT1501_CELLULAR_HPP
