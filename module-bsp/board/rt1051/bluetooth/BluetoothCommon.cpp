#include "bsp/bluetooth/Bluetooth.hpp"
#include "log/log.hpp"
#include "FreeRTOS.h"
#include "fsl_lpuart.h"
#include "board.h"

using namespace bsp;

// TODO it's plain copy same as in cellular - this is kind of wrong
uint32_t UartGetPeripheralClock();

void BTdev::_circ::sem_take()
{
    if (!(SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)) {
        xSemaphoreTake(sem, 0);
    }
    else {
        BaseType_t px;
        xSemaphoreTakeFromISR(sem, &px);
    }
}

void BTdev::_circ::sem_give()
{
    if (!(SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)) {
        xSemaphoreGive(sem);
    }
    else {
        BaseType_t px;
        xSemaphoreGiveFromISR(sem, &px);
    }
}

BTdev::_circ::_circ(unsigned int size, int threshold) : head(0), tail(0), threshold(threshold), size(size), len(0)
{
    buff = new char[size];
    sem  = xSemaphoreCreateBinary();
}

BTdev::_circ::~_circ()
{
    vSemaphoreDelete(sem);
    delete[] buff;
}

BluetoothCommon::BluetoothCommon(unsigned int in_size, unsigned int out_size, int threshold)
    : BTdev(in_size, out_size, threshold)
{
    configure_uart_io();
    configure_lpuart();
    configure_cts_irq();
    LOG_INFO("Bluetooth HW init done!");
}

BluetoothCommon::~BluetoothCommon()
{}

void BluetoothCommon::open()
{
    LOG_INFO("Bluetooth HW open!");
    set_reset(true);
    set_irq(true);
    is_open = true;
    set_rts(true);
}

void BluetoothCommon::close()
{
    LOG_INFO("close!");
    // TODO destroy semaphore
    set_rts(false);
    set_irq(false);
    is_open = false;
    set_reset(false);
}

void BluetoothCommon::sleep_ms(ssize_t ms)
{
    ulTaskNotifyTake(pdTRUE, ms);
}

BTdev::Error BluetoothCommon::flush()
{
    // LOG_INFO("flush [%d] %s", out.len, out.tail<out.head?"reverse":"normal");
    Error err  = Success;
    int len    = out.len;
    char *from = new char[out.len];
    for (int i = 0; i < len; ++i) {
        out.pop(from + i);
    }
    int to_write = len;
    char *fromp  = from;
    while (to_write) {
        while (1) {
            if (read_cts() == 0) {
                break;
            }
            else {
                sleep_ms(1);
            }
        }
        LPUART_WriteBlocking(BSP_BLUETOOTH_UART_BASE, reinterpret_cast<uint8_t *>(fromp), 1);
        --to_write;
        ++fromp;
    }
    delete[] from;
    return err;
}

ssize_t BluetoothCommon::write(char *buf, size_t nbytes)
{
    // LOG_INFO( "write -> [%.*s]",nbytes, buf);
    ssize_t i = 0;
    // if CTS set -> ignore return 0, can use threshold_guard here too
    for (i = 0; i < nbytes; ++i) {
        if (out.push(*(buf + i)) != 0) {
            LOG_ERROR("Cant push!");
            break;
        }
    }
    return i;
}

ssize_t BluetoothCommon::write_blocking(char *buf, ssize_t len)
{
    int yet_to_write = len;
    if (len > out.size) {
        LOG_WARN("WRITE: %d vs %d", len, out.size);
    }
    while (yet_to_write != 0) {
        yet_to_write -= write(buf + len - yet_to_write, yet_to_write < out.size ? yet_to_write : (out.size - 1));
        flush();
    }
    return len;
}

BTdev::Error BluetoothCommon::set_baudrate(uint32_t bd)
{
    LOG_INFO("Set baudrate: %" PRIu32, bd);
    Error ret = Success;
    int err   = 0;
    if ((err = LPUART_SetBaudRate(BSP_BLUETOOTH_UART_BASE, bd, UartGetPeripheralClock())) != 0) {
        LOG_ERROR("BT error: baudrate [%lu] set err: %d", bd, err);
        ret = ErrorBSP;
    }
    return ret;
}

// set flow on -> true, set flow off -> false
BTdev::Error BluetoothCommon::set_rts(bool on)
{
    GPIO_PinWrite(BSP_BLUETOOTH_UART_RTS_PORT, BSP_BLUETOOTH_UART_RTS_PIN, on ? 0U : 1U);
    return Success;
}

BTdev::Error BluetoothCommon::set_reset(bool on)
{
    LOG_INFO("reset %s", on ? "on" : "off");
    GPIO_PinWrite(BSP_BLUETOOTH_SHUTDOWN_PORT, BSP_BLUETOOTH_SHUTDOWN_PIN, on ? 1U : 0U);
    return Success;
}

int BluetoothCommon::read_cts()
{
    return GPIO_PinRead(BSP_BLUETOOTH_UART_CTS_PORT, BSP_BLUETOOTH_UART_CTS_PIN);
}

uint32_t UartGetPeripheralClock()
{
    const int UART_PERIPHERAL_PLL_DIVIDER = 6;
    uint32_t freq                         = 0;
    /* To make it simple, we assume default PLL and divider settings, and the only variable
       from application is use PLL3 source or OSC source */
    if (CLOCK_GetMux(kCLOCK_UartMux) == 0) /* PLL3 div6 80M */
    {
        freq = (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / UART_PERIPHERAL_PLL_DIVIDER) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
    }
    else {
        freq = CLOCK_GetOscFreq() / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U);
    }

    return freq;
}

void BluetoothCommon::configure_uart_io()
{
    gpio_pin_config_t gpio_init_structure;
    gpio_init_structure.direction     = kGPIO_DigitalOutput;
    gpio_init_structure.interruptMode = kGPIO_IntRisingOrFallingEdge;
    gpio_init_structure.outputLogic   = 1;
    GPIO_PinInit(BSP_BLUETOOTH_UART_RTS_PORT, BSP_BLUETOOTH_UART_RTS_PIN, &gpio_init_structure);
    gpio_init_structure.direction     = kGPIO_DigitalInput;
    gpio_init_structure.interruptMode = kGPIO_IntRisingOrFallingEdge;
    gpio_init_structure.outputLogic   = 0;
    GPIO_PinInit(BSP_BLUETOOTH_UART_CTS_PORT, BSP_BLUETOOTH_UART_CTS_PIN, &gpio_init_structure);
    gpio_init_structure.direction     = kGPIO_DigitalOutput;
    gpio_init_structure.interruptMode = kGPIO_NoIntmode;
    GPIO_PinInit(BSP_BLUETOOTH_OSC_EN_PORT, BSP_BLUETOOTH_OSC_EN_PIN, &gpio_init_structure);
    GPIO_PinWrite(BSP_BLUETOOTH_OSC_EN_PORT, BSP_BLUETOOTH_OSC_EN_PIN, 1U);
    gpio_init_structure.direction = kGPIO_DigitalOutput;
    GPIO_PinInit(BSP_BLUETOOTH_SHUTDOWN_PORT, BSP_BLUETOOTH_SHUTDOWN_PIN, &gpio_init_structure);
}

void BluetoothCommon::configure_lpuart()
{
    lpuart_config_t bt_c;
    LPUART_GetDefaultConfig(&bt_c);
    bt_c.baudRate_Bps  = baudrate;
    bt_c.dataBitsCount = kLPUART_EightDataBits;
    bt_c.parityMode    = kLPUART_ParityDisabled;
    bt_c.isMsb         = false;
    bt_c.rxIdleType    = kLPUART_IdleTypeStartBit;
    bt_c.rxIdleConfig  = kLPUART_IdleCharacter1;
    bt_c.enableTx      = false;
    bt_c.enableRx      = false;

    if (LPUART_Init(BSP_BLUETOOTH_UART_BASE, &bt_c, UartGetPeripheralClock()) != kStatus_Success) {
        LOG_ERROR("BT: UART config error Could not initialize the uart!");
        return;
    }

    LPUART_ClearStatusFlags(BSP_BLUETOOTH_UART_BASE, 0xFFFFFFFF);
    NVIC_ClearPendingIRQ(LPUART2_IRQn);
    NVIC_SetPriority(LPUART2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(LPUART2_IRQn);
}

void BluetoothCommon::configure_cts_irq()
{
    DisableIRQ(GPIO1_Combined_16_31_IRQn);
    GPIO_PortEnableInterrupts(GPIO1, 0xFFFFFFFF);
    // Clear all IRQs
    GPIO_PortClearInterruptFlags(GPIO1, 0xFFFFFFFF);
    GPIO_PortDisableInterrupts(GPIO1, 0xFFFFFFFF);
    EnableIRQ(GPIO1_Combined_16_31_IRQn);
    NVIC_SetPriority(GPIO1_Combined_16_31_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
}

void BluetoothCommon::set_irq(bool enable)
{
    // printf("%s\n", __FUNCTION__);
    LPUART_EnableRx(BSP_BLUETOOTH_UART_BASE, false);
    LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, false);
    LPUART_ClearStatusFlags(BSP_BLUETOOTH_UART_BASE, 0xFFFFFFFF);
    if (enable) {
        LPUART_EnableInterrupts(BSP_BLUETOOTH_UART_BASE, kLPUART_RxDataRegFullInterruptEnable);
    }
    else {
        LPUART_DisableInterrupts(BSP_BLUETOOTH_UART_BASE, kLPUART_RxDataRegFullInterruptEnable);
    }
    // LPUART_EnableInterrupts(BSP_BLUETOOTH_UART_BASE,
    // kLPUART_RxDataRegFullInterruptEnable|kLPUART_TxDataRegEmptyInterruptEnable|kLPUART_TransmissionCompleteInterruptEnable|kLPUART_RxOverrunInterruptEnable
    // );
    LPUART_EnableRx(BSP_BLUETOOTH_UART_BASE, true);
    LPUART_EnableTx(BSP_BLUETOOTH_UART_BASE, true);
}

extern "C"
{
    void GPIO1_Combined_16_31_IRQHandler(void)
    {
        uint32_t irq_mask = GPIO_GetPinsInterruptFlags(GPIO2);
        if (irq_mask & (1 << BSP_BLUETOOTH_UART_CTS_PIN)) {
            // printf("CTS IRQ!\n");
        }
        GPIO_PortClearInterruptFlags(GPIO1, irq_mask);
    }
};
