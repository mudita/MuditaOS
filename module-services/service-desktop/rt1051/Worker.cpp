#include "Worker.hpp"

extern "C"
{
#include "include/virtual_com.h"
}

int desktopServiceInit()
{
    return (VCOMAPPInit());
}

void desktopServiceReceive(void *ptr)
{
    ServiceDesktop *owner = (ServiceDesktop *)ptr;
    static uint8_t readbuf[SERIAL_BUFFER_LEN];

    while (1)
    {
        ssize_t length = USB_CDCGetReceived(readbuf);

        if (length > 0)
        {
            owner->dataReceived(readbuf, length);
            USB_CDCSend(NULL, 0);
        }
        else if (length == -1)
        {
            LOG_ERROR("error receiving usb data");
        }
    }
}

/* fd is not needed on pure phone */
int desktopServiceSend(int fd, uint8_t *data, size_t dataSize)
{
    usb_status_t t = USB_CDCSend(data, dataSize);
    if (t == 0x00)
    {
        return (dataSize);
    }
    else
    {
        LOG_ERROR("USB_CDCSend failed with code: %d", t);
        return (-1);
    }
}
