#include "bsp/usb_cdc/usb_cdc.hpp"

extern "C"
{
#include "driver/include/virtual_com.h"
}

namespace bsp
{
    xQueueHandle USBReceiveQueue;

    void usbCDCReceive(void *)
    {
        LOG_INFO("[ServiceDesktop:BSP_Driver] Start reading USB_CDC");
        uint8_t inputData[SERIAL_BUFFER_LEN];
        static std::string receive_msg;

        while (1) {
            if (uxQueueSpacesAvailable(USBReceiveQueue) != 0) {

                ssize_t length = USB_CDCGetReceived(inputData);

                if (length > 0) {
                    static std::string receive_msg;
                    receive_msg = std::string(inputData, inputData + length);
                    LOG_DEBUG("[ServiceDesktop:BSP_Driver] Received: %d signs", length);
                    xQueueSend(USBReceiveQueue, &receive_msg, portMAX_DELAY);

                    USB_CDCSend(NULL, 0);
                }
                else if (length == -1) {
                    LOG_ERROR("[ServiceDesktop:BSP_Driver] Error receiving usb data");
                }
            }
            else {
                LOG_DEBUG("[ServiceDesktop:BSP_Driver] USB receive Queue is full, yielding task");
                vTaskDelay(1000);
            }
        }
    }

    int usbCDCSend(std::string *send_msg)
    {

        usb_status_t t = USB_CDCSend((uint8_t *)(*send_msg).c_str(), (*send_msg).length());
        delete send_msg;

        if (t == 0x00) {
            LOG_DEBUG("[ServiceDesktop:BSP_Driver] Send: %d signs", (*send_msg).length());
            return 0;
        }
        else {
            LOG_ERROR("[ServiceDesktop:BSP_Driver] USB_CDCSend failed with code: %d", t);
            return -1;
        }
    }

    int usbCDCInit(xQueueHandle receiveQueue)
    {
        int ret = VCOMAPPInit();

        xTaskHandle taskHandleReceive;
        USBReceiveQueue = receiveQueue;

        BaseType_t task_error = xTaskCreate(
            usbCDCReceive, "USBRT1051Receive", SERIAL_BUFFER_LEN * 8, (void *)1, tskIDLE_PRIORITY, &taskHandleReceive);

        if (task_error != pdPASS) {
            LOG_ERROR("[ServiceDesktop:BSP_Driver] Failed to start freertos USB_RT1051_Receive");
        }

        return ret;
    }

    void usbCDCFlush()
    {
        USB_CDCSend(NULL, 0);
    }
} // namespace bsp
