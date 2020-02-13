#include "Worker.hpp"

extern xQueueHandle queueHandleBuffer;

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
    //    ServiceDesktop *owner = (ServiceDesktop *)ptr;
    //    static uint8_t inputData[SERIAL_BUFFER_LEN];
    //
    //    while (1)
    //    {
    //        ssize_t length = USB_CDCGetReceived(inputData);
    //
    //        if (length > 0)
    //        {
    //            owner->dataReceived(inputData, length);
    //            xQueueSend(queueHandleBuffer, inputData, portMAX_DELAY);
    //            USB_CDCSend(NULL, 0);
    //        }
    //        else if (length == -1)
    //        {
    //            LOG_ERROR("error receiving usb data");
    //        }
    //    }
}

int desktopServiceSend(std::string msg)
{
    //    static uint8_t outputData[SERIAL_BUFFER_LEN];
    //
    //    if (xQueueReceive(queueHandleBuffer, outputData, portMAX_DELAY))
    //    {
    //        usb_status_t t = USB_CDCSend(outputData, sizeof(outputData));
    //        if (t == 0x00)
    //        {
    //            LOG_DEBUG("Send: %d signs", sizeof(outputData));
    //        }
    //        else
    //        {
    //            LOG_ERROR("USB_CDCSend failed with code: %d", t);
    //        }
    //    }
    return 0;
}
