
/*
 * @file rt1501_cellular.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 17.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "rt1051_cellular.hpp"

#include "FreeRTOS.h"
#include "task.h"

#include "dma_config.h"
#include "board.h"


void LPUART1_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    uint32_t isrReg = LPUART_GetStatusFlags(CELLULAR_UART_BASE);
    static char characterReceived  = 0;

    if (bsp::RT1051Cellular::uartRxStreamBuffer != NULL)
    {
        if(isrReg & kLPUART_RxDataRegFullFlag)
        {
            characterReceived = LPUART_ReadByte(CELLULAR_UART_BASE);

            if (characterReceived != 0)
            {
                if (xStreamBufferSpacesAvailable(bsp::RT1051Cellular::uartRxStreamBuffer) < 8)
                {
                    //BSP_CellularDeassertRTS();
                }

                xStreamBufferSendFromISR( bsp::RT1051Cellular::uartRxStreamBuffer,
                                          ( void * ) &characterReceived,
                                          1,
                                          &xHigherPriorityTaskWoken );
            }

        }
    }
    LPUART_ClearStatusFlags(CELLULAR_UART_BASE, isrReg);


    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

namespace bsp {

    RT1051Cellular::RT1051Cellular() {

        MSPInit();
        DMAInit();

        uartRxStreamBuffer = xStreamBufferCreate(rxStreamBufferLength, rxStreamBufferNotifyWatermark);
        if (uartRxStreamBuffer == NULL)
        {
            LOG_ERROR("Cellular Uart error: Could not create the RX stream buffer!");
            return;
        }

        lpuart_config_t s_cellularConfig;

        LPUART_GetDefaultConfig(&s_cellularConfig);

        s_cellularConfig.baudRate_Bps   = baudrate;
        s_cellularConfig.dataBitsCount  = kLPUART_EightDataBits;
        s_cellularConfig.parityMode     = kLPUART_ParityDisabled;
        s_cellularConfig.isMsb          = false;
        s_cellularConfig.rxIdleType     = kLPUART_IdleTypeStartBit;
        s_cellularConfig.rxIdleConfig   = kLPUART_IdleCharacter1;
        s_cellularConfig.enableTx       = false;
        s_cellularConfig.enableRx       = false;

        if (LPUART_Init(CELLULAR_UART_BASE, &s_cellularConfig, UartGetPeripheralClock()) != kStatus_Success)
        {
            LOG_ERROR("Cellular Uart config error: Could not initialize the uart!");
            return;
        }

        LPUART_ClearStatusFlags(CELLULAR_UART_BASE, 0xFFFFFFFF);
        NVIC_ClearPendingIRQ(LPUART1_IRQn);
        NVIC_SetPriority(LPUART1_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
        NVIC_EnableIRQ(LPUART1_IRQn);

        EnableRx();
    }


    RT1051Cellular::~RT1051Cellular() {

        if(uartRxStreamBuffer){
            vStreamBufferDelete(uartRxStreamBuffer);
        }

        DisableRx();

        NVIC_DisableIRQ(LPUART1_IRQn);
        LPUART_ClearStatusFlags(CELLULAR_UART_BASE, 0xFFFFFFFF);
        NVIC_ClearPendingIRQ(LPUART1_IRQn);

        LPUART_Deinit(CELLULAR_UART_BASE);

    }


    void RT1051Cellular::MSPInit() {
        gpio_pin_config_t gpio_init_structure;

        gpio_init_structure.direction       = kGPIO_DigitalOutput;
        gpio_init_structure.interruptMode   = kGPIO_NoIntmode;

        /*
         *      CELLULAR OUTPUT PINS WITH INITIAL VALUE 1
         */
        gpio_init_structure.outputLogic     = 1;


        /*
         *      CELLULAR OUTPUT PINS WITH INITIAL VALUE 0
         */
        gpio_init_structure.outputLogic     = 0;

        GPIO_PinInit(BSP_CELLULAR_UART_RTS_PORT,            BSP_CELLULAR_UART_RTS_PIN,          &gpio_init_structure);
        GPIO_PinInit(BSP_CELLULAR_UART_DTR_PORT,            BSP_CELLULAR_UART_DTR_PIN,          &gpio_init_structure);
        GPIO_PinInit(BSP_CELLULAR_AP_RDY_PORT,              BSP_CELLULAR_AP_RDY_PIN,            &gpio_init_structure);
        GPIO_PinInit(BSP_CELLULAR_POWER_PORT,               BSP_CELLULAR_POWER_PIN,             &gpio_init_structure);
        GPIO_PinInit(BSP_CELLULAR_RESET_PORT,               BSP_CELLULAR_RESET_PIN,             &gpio_init_structure);
        GPIO_PinInit(BSP_CELLULAR_WAKEUP_PORT,              BSP_CELLULAR_WAKEUP_PIN,            &gpio_init_structure);
        GPIO_PinInit(BSP_CELLULAR_SIMSEL_PORT,              BSP_CELLULAR_SIMSEL_PIN,            &gpio_init_structure);
        GPIO_PinInit(BSP_CELLULAR_SIM_CARD_PRESENCE_PORT,   BSP_CELLULAR_SIM_CARD_PRESENCE_PIN, &gpio_init_structure);

        /*
         *      CELLULAR INPUT PINS
         */
        gpio_init_structure.direction       = kGPIO_DigitalInput;
        gpio_init_structure.interruptMode   = kGPIO_IntRisingOrFallingEdge;
        gpio_init_structure.outputLogic     = 1;    //< Don't care for input

        GPIO_PinInit(BSP_CELLULAR_RI_PORT,                      BSP_CELLULAR_RI_PIN,                    &gpio_init_structure);
        GPIO_PinInit(BSP_CELLULAR_UART_CTS_PORT,                BSP_CELLULAR_UART_CTS_PIN,              &gpio_init_structure);
        GPIO_PinInit(BSP_CELLULAR_SIM_CARD_1_INSERTED_PORT,     BSP_CELLULAR_SIM_CARD_1_INSERTED_PIN,   &gpio_init_structure);
        GPIO_PinInit(BSP_CELLULAR_SIM_CARD_2_INSERTED_PORT,     BSP_CELLULAR_SIM_CARD_2_INSERTED_PIN,   &gpio_init_structure);

        GPIO_PortEnableInterrupts(BSP_CELLULAR_SIM_CARD_1_INSERTED_PORT, 1U << BSP_CELLULAR_SIM_CARD_1_INSERTED_PIN);
        GPIO_PortEnableInterrupts(BSP_CELLULAR_SIM_CARD_2_INSERTED_PORT, 1U << BSP_CELLULAR_SIM_CARD_2_INSERTED_PIN);
    }

    void RT1051Cellular::DMAInit() {
        DMAMUX_SetSource(BSP_CELLULAR_UART_TX_DMA_DMAMUX_BASE, BSP_CELLULAR_UART_TX_DMA_CH, kDmaRequestMuxLPUART1Tx);

        DMAMUX_EnableChannel(BSP_CELLULAR_UART_TX_DMA_DMAMUX_BASE, BSP_CELLULAR_UART_TX_DMA_CH);

        EDMA_CreateHandle(&uartTxDmaHandle, BSP_CELLULAR_UART_TX_DMA_BASE, BSP_CELLULAR_UART_TX_DMA_CH);

        LPUART_TransferCreateHandleEDMA(CELLULAR_UART_BASE,
                                        &uartDmaHandle,
                                        DMATxCompletedCb,
                                        NULL,
                                        &uartTxDmaHandle,
                                        NULL);
    }

    uint32_t RT1051Cellular::UartGetPeripheralClock() {
        const int UART_PERIPHERAL_PLL_DIVIDER = 6;
        uint32_t freq = 0;
        /* To make it simple, we assume default PLL and divider settings, and the only variable
           from application is use PLL3 source or OSC source */
        if (CLOCK_GetMux(kCLOCK_UartMux) == 0) /* PLL3 div6 80M */
        {
            freq = (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / UART_PERIPHERAL_PLL_DIVIDER) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
        }
        else
        {
            freq = CLOCK_GetOscFreq() / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
        }

        return freq;
    }

    void RT1051Cellular::EnableRx() {
        LPUART_ClearStatusFlags(CELLULAR_UART_BASE, 0xFFFFFFFF);
        LPUART_EnableInterrupts(CELLULAR_UART_BASE, kLPUART_RxDataRegFullInterruptEnable);
        LPUART_EnableRx(CELLULAR_UART_BASE, true);
    }

    void RT1051Cellular::DisableRx() {
        LPUART_DisableInterrupts(CELLULAR_UART_BASE, kLPUART_RxDataRegFullInterruptEnable);
        LPUART_ClearStatusFlags(CELLULAR_UART_BASE, kLPUART_RxDataRegFullInterruptEnable);
        LPUART_EnableRx(CELLULAR_UART_BASE, false);
    }

    void RT1051Cellular::DMATxCompletedCb(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status,
                                                   void *userData) {

        BaseType_t higherPriorTaskWoken = 0;
        xTaskNotifyFromISR((TaskHandle_t)userData, 0, eNoAction, &higherPriorTaskWoken);

        portEND_SWITCHING_ISR(higherPriorTaskWoken);
    }

}