/*
 * Copyright  Onplick <info@onplick.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
#ifndef _USB_CDC_VCOM_H_
#define _USB_CDC_VCOM_H_

/* Currently configured line coding */
#define LINE_CODING_DTERATE (115200)
#define LINE_CODING_CHARFORMAT (0x00)
#define LINE_CODING_PARITYTYPE (0x00)
#define LINE_CODING_DATABITS (0x08)

/* Communications feature */
#define COMM_FEATURE_DATA_SIZE (0x02)
#define STATUS_ABSTRACT_STATE (0x0000)
#define COUNTRY_SETTING (0x0000)

/* Notification of serial state */
#define NOTIF_PACKET_SIZE (0x08)
#define UART_BITMAP_SIZE (0x02)
#define NOTIF_REQUEST_TYPE (0xA1)

#define VCOM_INPUT_STREAM_SIZE (512)
#define VCOM_OUTPUT_STREAM_SIZE (512)

/* Events */
enum vcomEvent {
    VCOM_ATTACHED,
    VCOM_CONFIGURED,
    VCOM_DETACHED,
    VCOM_RESET,
    VCOM_ERROR_MALFORMED_USB_PACKET,
    VCOM_ERROR_MISSED_INCOMING_DATA,
    VCOM_ERROR_RX_BUFFER_OVERFLOW,
    VCOM_ERROR_TX_BUFFER_OVERFLOW,
    VCOM_WARNING_RESCHEDULE_BUSY,
    VCOM_WARNING_NOT_CONFIGURED,
};

/* @brief Function to inform user code about events happening in
 *        virtual com module.
 * @NOTE: function may be called from ISR context. */
typedef void (*userCbFunc)(void *userArg, enum vcomEvent);

/* Define the types for application */
typedef struct _usb_cdc_vcom_struct
{
    class_handle_t cdcAcmHandle;    /* USB CDC ACM class handle. */
    uint8_t configured;             /* Host is connected and interface is configured */
    uint8_t startTransactions;      /* A flag to indicate whether a CDC device is ready to transmit and receive data. */
    uint8_t currentConfiguration;   /* Current configuration value. */

    StreamBufferHandle_t inputStream;
    StreamBufferHandle_t outputStream;

    /* Event callback function prototype */
    userCbFunc userCb;
    void *userCbArg;
} usb_cdc_vcom_struct_t;



/*!
 * @brief Virtual COM device initialization function.
 *
 * This function initializes the device with the composite device class information.
 *
 * @param cdcVcom The pointer to the structure of CDC Virtual Com
 * @param classHandle handle to lower layer USB class representation
 * @param callback optional function for notifications
 * @param userArg optional argument for user callback
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t VirtualComInit(usb_cdc_vcom_struct_t *cdcVcom, class_handle_t classHandle, userCbFunc callback, void *userArg);

/**
 * @brief Notify VirtualCom that USB cable is unplugged
 */
void VirtualComDetached(usb_cdc_vcom_struct_t *cdcVcom);

/**
 * @brief Notify VirtualCom that bus reset event occured
 */
void VirtualComReset(usb_cdc_vcom_struct_t *cdcVcom);

/**
 * @brief Queue data to send
 * @param data buffer. It would be copied into stream memory
 * @param length of data
 * @reutrn negative value on error (usb subsystem status code),
 *         number bytes enqueued to send
 */
int VirtualComSend(usb_cdc_vcom_struct_t *cdcVcom, const void* data, size_t length);

/**
 * @brief Pick received data from stream
 * @param Buffer where data would be copied from stream
 * @return length of data, 0 if data not available,
 */
int VirtualComRecv(usb_cdc_vcom_struct_t *cdcVcom, void* data, size_t length);

/*!
 * @brief Handles events comming from USB sub system
 *
 * @param handle          The CDC ACM class handle.
 * @param event           The CDC ACM class event type.
 * @param param           The parameter of the class specific request.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t VirtualComUSBCallback(uint32_t event, void *param, void *userArg);

/*!
 * @brief Virtual COM device set configuration function.
 *
 * This function sets configuration for CDC class.
 *
 * @param handle The CDC ACM class handle.
 * @param configure The CDC ACM class configure index.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t VirtualComUSBSetConfiguration(usb_cdc_vcom_struct_t *cdcVcom, uint8_t configure);


#endif /* _USB_CDC_VCOM_H_ */
