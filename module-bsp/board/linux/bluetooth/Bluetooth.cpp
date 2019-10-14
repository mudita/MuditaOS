#include "bsp/bluetooth/Bluetooth.hpp"
#include "BluetoothWorker.hpp"
#include "log/log.hpp"

/// stubs

using namespace bsp;

BlueKitchen::BlueKitchen(unsigned int in_size, unsigned int out_size) :BluetoothCommon(in_size,out_size)
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

ssize_t BlueKitchen::write_blocking(char *buf, ssize_t len)
{
    return 0;
}

BluetoothCommon::BluetoothCommon(unsigned int in_size, unsigned int out_size, int threshold):
    BTdev(in_size,out_size,threshold)
{
}

BlueKitchen::~BlueKitchen() {
}

BluetoothCommon::~BluetoothCommon()
{
}

void BluetoothCommon::open()
{
}

void BluetoothCommon::close()
{
}

void BluetoothCommon::sleep_ms(ssize_t ms)
{
    ulTaskNotifyTake(pdTRUE, ms);
}

BTdev::Error BluetoothCommon::flush()
{
    return Success;
}

ssize_t BluetoothCommon::write(char *buf, size_t nbytes)
{
    return 0;
}

ssize_t BluetoothCommon::write_blocking(char *buf, ssize_t len)
{
    return 0;
}

BTdev::Error BluetoothCommon::set_baudrate(uint32_t bd)
{
    return Success;
}

// set flow on -> true, set flow off -> false
BTdev::Error BluetoothCommon::set_rts(bool on)
{
    return Success;
}

BTdev::Error BluetoothCommon::set_reset(bool on)
{
    return Success;
}

int BluetoothCommon::read_cts()
{
    return 0;
}

void BluetoothCommon::configure_uart_io()
{
}

void BluetoothCommon::configure_lpuart()
{
}

void BluetoothCommon::configure_cts_irq()
{
}

void BluetoothCommon::set_irq(bool enable)
{
}

ssize_t BlueKitchen::read(void *buf, size_t nbytes)
{
}

ssize_t Bluetopia::read(void *buf, size_t nbytes)
{
}
