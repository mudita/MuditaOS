#include "bsp_bt.hpp"
#include "log/log.hpp"

namespace bsp {

BluetopiaHW::BluetopiaHW(LogLvl lvl) : Bluetopia(default_buff_size,default_buff_size,32)
{
    set_logging(lvl);
    configure_uart_io();
    configure_lpuart();
    configure_cts_irq();
    LOG_INFO("Init done!");
}

BluetopiaHW::~BluetopiaHW() {
}

Bluetopia *BluetopiaHW::getInstance()
{
    static BluetopiaHW *inst = NULL;
    if(inst==NULL) {
        inst = new BluetopiaHW();
    }
    return inst;
}

void BluetopiaHW::open()
{
    LOG_INFO("open!");
    vSemaphoreCreateBinary(sem_data);
    set_irq(true);
    is_open = true;
    set_rts(true);
}

void BluetopiaHW::close()
{
    LOG_INFO("close!");
    set_rts(false);
    set_irq(false);
    is_open = false;
}

void BluetopiaHW::sleep_ms(ssize_t ms)
{
    ulTaskNotifyTake(pdTRUE, ms);
}

BTdev::Error BluetopiaHW::flush()
{
    // LOG_INFO("flush [%d] %s", out.len, out.tail<out.head?"reverse":"normal");
    Error err = Success;
    int i=0;
    for(i=0; i<default_timeout_ms; ++i) {
        if(read_cts() == 0) {
            break;
        } else {
            sleep_ms(1);
        }
    }
    if(i==default_timeout_ms) {
        LOG_ERROR("BT CTS error!");
        err = ErrorTimeout;
    }
    char* from = &out.buff[out.head];
    if(out.tail > out.head) {
        LPUART_WriteBlocking(BSP_BLUETOOTH_UART_BASE, reinterpret_cast<uint8_t*>(from), out.len);
    } else {
        LPUART_WriteBlocking(BSP_BLUETOOTH_UART_BASE, reinterpret_cast<uint8_t*>(from), out.len - out.tail);
        from = out.buff;
        LPUART_WriteBlocking(BSP_BLUETOOTH_UART_BASE, reinterpret_cast<uint8_t*>(from), out.tail);
    }
    out.flush();
    return err;
}

ssize_t BluetopiaHW::write(char *buf, size_t nbytes)
{
    // LOG_INFO( "write -> [%.*s]",nbytes, buf);
    ssize_t i=0;
    // if CTS set -> ignore return 0, can use threshold_guard here too
    for (i=0; i<nbytes; ++i) {
        if(out.push(*(buf+i)) != 0) {
            break;
        }
    }
    return i;
}

ssize_t BluetopiaHW::write_blocking(char *buf, ssize_t len)
{
    int llen = 0;
    while (llen != len) {
        llen += write(buf, len);
        // no error check here!
        flush();
    }
    return llen;
}

// TODO it's plain copy same as in cellular - this is kind of wrong
uint32_t BluetopiaHW::UartGetPeripheralClock()
{
    const int UART_PERIPHERAL_PLL_DIVIDER = 6;
    uint32_t freq = 0;
    /* To make it simple, we assume default PLL and divider settings, and the only variable
       from application is use PLL3 source or OSC source */
    if (CLOCK_GetMux(kCLOCK_UartMux) == 0) /* PLL3 div6 80M */
    {
        freq = (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / UART_PERIPHERAL_PLL_DIVIDER) /
            (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
    } else {
        freq = CLOCK_GetOscFreq() / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
    }

    return freq;
}


BTdev::Error BluetopiaHW::set_baudrate(uint32_t bd)
{
    LOG_INFO( "Set baudrate: %d", bd);
    Error ret = Success;
    int err=0;
    if((err=LPUART_SetBaudRate(BSP_BLUETOOTH_UART_BASE, bd, UartGetPeripheralClock()))!=0) {
        LOG_ERROR("BT error: baudrate [%lu] set err: %d", bd, err);
        ret = ErrorBSP;
    }
    return ret;
}

// set flow on -> true, set flow off -> false
BTdev::Error BluetopiaHW::set_rts(bool on)
{
    GPIO_PinWrite(BSP_BLUETOOTH_UART_RTS_PORT, BSP_BLUETOOTH_UART_RTS_PIN, on?0U:1U);
    return Success;
}

BTdev::Error BluetopiaHW::set_reset(bool on)
{
    LOG_INFO("reset %s", on?"on":"off");
    GPIO_PinWrite(BSP_BLUETOOTH_SHUTDOWN_PORT, BSP_BLUETOOTH_SHUTDOWN_PIN, on?1U:0U);
    return Success;
}

int BluetopiaHW::read_cts()
{
    return GPIO_PinRead(BSP_BLUETOOTH_UART_CTS_PORT, BSP_BLUETOOTH_UART_CTS_PIN);
}

ssize_t BluetopiaHW::read(void*, size_t nbytes)
{
    LOG_INFO("not implemented");
    return 0;
}

/// ---- PRIVATES

void BluetopiaHW::configure_uart_io()
{
    gpio_pin_config_t gpio_init_structure;
    gpio_init_structure.direction       = kGPIO_DigitalOutput;
    gpio_init_structure.interruptMode   = kGPIO_IntRisingOrFallingEdge;
    gpio_init_structure.outputLogic     = 1;
    GPIO_PinInit(BSP_BLUETOOTH_UART_RTS_PORT,            BSP_BLUETOOTH_UART_RTS_PIN,          &gpio_init_structure);
    gpio_init_structure.direction       = kGPIO_DigitalInput;
    gpio_init_structure.interruptMode   = kGPIO_IntRisingOrFallingEdge;
    gpio_init_structure.outputLogic     = 0;
    GPIO_PinInit(BSP_BLUETOOTH_UART_CTS_PORT,            BSP_BLUETOOTH_UART_CTS_PIN,          &gpio_init_structure);
    gpio_init_structure.direction       = kGPIO_DigitalOutput;
    gpio_init_structure.interruptMode   = kGPIO_NoIntmode;
    GPIO_PinInit(BSP_BLUETOOTH_OSC_EN_PORT, BSP_BLUETOOTH_OSC_EN_PIN ,&gpio_init_structure);
    GPIO_PinWrite(BSP_BLUETOOTH_OSC_EN_PORT, BSP_BLUETOOTH_OSC_EN_PIN, 1U);
    gpio_init_structure.direction       = kGPIO_DigitalOutput;
    GPIO_PinInit(BSP_BLUETOOTH_SHUTDOWN_PORT, BSP_BLUETOOTH_SHUTDOWN_PIN ,&gpio_init_structure);
}

void BluetopiaHW::configure_lpuart()
{
    lpuart_config_t bt_c;
    LPUART_GetDefaultConfig(&bt_c);
    bt_c.baudRate_Bps   = baudrate;
    bt_c.dataBitsCount  = kLPUART_EightDataBits;
    bt_c.parityMode     = kLPUART_ParityDisabled;
    bt_c.isMsb          = false;
    bt_c.rxIdleType     = kLPUART_IdleTypeStartBit;
    bt_c.rxIdleConfig   = kLPUART_IdleCharacter1;
    bt_c.enableTx       = false;
    bt_c.enableRx       = false;

    if (LPUART_Init(BSP_BLUETOOTH_UART_BASE, &bt_c, UartGetPeripheralClock()) != kStatus_Success) {
        LOG_ERROR("BT: UART config error Could not initialize the uart!");
        return;
    }

    LPUART_ClearStatusFlags(BSP_BLUETOOTH_UART_BASE, 0xFFFFFFFF);
    NVIC_ClearPendingIRQ(LPUART2_IRQn);
    NVIC_SetPriority(LPUART2_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(LPUART2_IRQn);
}

void BluetopiaHW::configure_cts_irq()
{
    DisableIRQ(GPIO1_Combined_16_31_IRQn);
    GPIO_PortEnableInterrupts(GPIO1, 0xFFFFFFFF);
    // Clear all IRQs
    GPIO_PortClearInterruptFlags(GPIO1, 0xFFFFFFFF);
    GPIO_PortDisableInterrupts(GPIO1, 0xFFFFFFFF);
    EnableIRQ(GPIO1_Combined_16_31_IRQn);
    NVIC_SetPriority(GPIO1_Combined_16_31_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
}

void BluetopiaHW::set_irq(bool enable)
{
    //printf("%s\n", __FUNCTION__);
    LPUART_EnableRx(BSP_BLUETOOTH_UART_BASE, false);
    LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, false);
    LPUART_ClearStatusFlags(BSP_BLUETOOTH_UART_BASE, 0xFFFFFFFF);
    if(enable) {
        LPUART_EnableInterrupts(BSP_BLUETOOTH_UART_BASE, kLPUART_RxDataRegFullInterruptEnable);
    } else {
        LPUART_DisableInterrupts(BSP_BLUETOOTH_UART_BASE, kLPUART_RxDataRegFullInterruptEnable);
    }
    //LPUART_EnableInterrupts(BSP_BLUETOOTH_UART_BASE, kLPUART_RxDataRegFullInterruptEnable|kLPUART_TxDataRegEmptyInterruptEnable|kLPUART_TransmissionCompleteInterruptEnable|kLPUART_RxOverrunInterruptEnable );
    LPUART_EnableRx(BSP_BLUETOOTH_UART_BASE, true);
    LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, true);
}

void BluetopiaHW::wait_data()
{
    while(in.len == 0) {
        // TODO checkme
        xSemaphoreTake(sem_data, -1);
    }
}

void BluetopiaHW::set_data()
{
    long tmp;
    xSemaphoreGiveFromISR(sem_data, &tmp);
}

};

extern "C" {
    /* IN LIB: now it's simple on RX instead
       > The following function handles the GPIO interrupt from the
       > Bluetooth CTS line.  will make sure that the transmit interrupt is
       > enabled when the flow control line is low.
       */
    void LPUART2_IRQHandler(void)
    {
        uint32_t isrReg = LPUART_GetStatusFlags(BSP_BLUETOOTH_UART_BASE);
        static char characterReceived  = 0;

        if(isrReg & kLPUART_RxDataRegFullFlag)
        {
            characterReceived = LPUART_ReadByte(BSP_BLUETOOTH_UART_BASE);
            bsp::Bluetopia *bt = bsp::BluetopiaHW::getInstance();
            if(bt->in.push(characterReceived) != 0) {
                // LOG_ERROR("BT: error no RX space!");
            } else {
                bt->set_data();
            }
            if(bt->in.threshold_guard()) {
                bt->set_rts(false);
            }
        }
        // TODO ths should be handled - othervise uart might be `nicelly` blocked
        if(isrReg & kLPUART_RxOverrunFlag) {
            printf("Overrun\n");
        }
        LPUART_ClearStatusFlags(BSP_BLUETOOTH_UART_BASE, isrReg);
    }

    void GPIO1_Combined_16_31_IRQHandler(void)
    {
        uint32_t irq_mask = GPIO_GetPinsInterruptFlags(GPIO2);
        if (irq_mask & (1 << BSP_BLUETOOTH_UART_CTS_PIN)) {
            // printf("CTS IRQ!\n");
        }
        GPIO_PortClearInterruptFlags(GPIO1, irq_mask);
    }
};
