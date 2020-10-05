#ifndef _USB_DEVICE_COMPOSITE_H_
#define _USB_DEVICE_COMPOSITE_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "message_buffer.h"
#include "event_groups.h"

#include "virtual_com.h"
#include "mtp.h"

#define CONTROLLER_ID kUSB_ControllerEhci0
#define USB_DEVICE_INTERRUPT_PRIORITY (3U)

typedef struct _usb_device_composite_struct
{
    usb_device_handle deviceHandle; /* USB device handle. */
    usb_cdc_vcom_struct_t cdcVcom;  /* CDC virtual com device structure. */
    usb_mtp_struct_t mtpApp;

    TaskHandle_t applicationTaskHandle; /* Application task handle. */
    TaskHandle_t deviceTaskHandle;      /* USB device task handle. */
    uint8_t speed;  /* Speed of USB device. USB_SPEED_FULL/USB_SPEED_LOW/USB_SPEED_HIGH.                 */
    uint8_t attach; /* A flag to indicate whether a usb device is attached. 1: attached, 0: not attached */
    uint8_t currentConfiguration; /* Current configuration value. */
    uint8_t
        currentInterfaceAlternateSetting[USB_INTERFACE_COUNT]; /* Current alternate setting value for each interface. */
} usb_device_composite_struct_t;

usb_cdc_vcom_struct_t *composite_init();

#endif /* _USB_DEVICE_COMPOSITE_H_ */
