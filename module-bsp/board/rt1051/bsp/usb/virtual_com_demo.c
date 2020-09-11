#include "usb.h"
#include "usb_device.h"
#include "usb_device_class.h"
#include "usb_device_cdc_acm.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"
#include "composite.h"

#include "virtual_com_demo.h"

static void VirtualComConsumerTask(void *handle)
{
#define RX_CHUNK 512
    char *input = pvPortMalloc(RX_CHUNK);
    usb_cdc_vcom_struct_t *cdcVcom = handle;
    size_t len;
    size_t total = 0;

    if (!input) {
        LOG_ERROR("Buffer allocation failed");
        return;
    }

    vTaskDelay(3000/portTICK_PERIOD_MS);

    LOG_INFO("Task started");

    while (1)
    {
        //if VirtualComConfigured() {
        //}
        len = VirtualComRecv(cdcVcom, input, RX_CHUNK);
        if (len > 0)
        {
            size_t sent = 0;
            total += len;
            do {
                size_t result = VirtualComSend(cdcVcom, &input[sent], len-sent);
                if (!result) {
                    continue;
                }
                sent += result;
            } while (sent < len);
        } else {
            vTaskDelay(1/portTICK_PERIOD_MS);
        }
    }
}

void VirtualComDemoInit(usb_cdc_vcom_struct_t *cdcVcom)
{
    if (xTaskCreate(VirtualComConsumerTask,
                    "VirtualComDemo",
                    2048L / sizeof(portSTACK_TYPE),
                    cdcVcom,
                    2,
                    NULL
                    ) != pdPASS)
    {
        LOG_ERROR("VirtualComDemo xTaskCreate failed");
    }
}

