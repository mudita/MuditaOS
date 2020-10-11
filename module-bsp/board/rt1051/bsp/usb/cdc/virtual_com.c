/*
 * Copyright  Onplick <info@onplick.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */
#include <stdlib.h>

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"
#include "usb_device_class.h"
#include "usb_device_cdc_acm.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"
#include "composite.h"

#define PRINTF LOG_DEBUG

#if (VCOM_INPUT_STREAM_SIZE < HS_CDC_VCOM_BULK_IN_PACKET_SIZE) || \
    (VCOM_OUTPUT_STREAM_SIZE < HS_CDC_VCOM_BULK_OUT_PACKET_SIZE)
#   error "VCOM stream size has to be greater than single USB packet length"
#endif

#define call_user_cb(handle, id) do { \
    if (handle->userCb) \
            handle->userCb(handle->userCbArg, id); \
    } while (0);


/* Define the information relates to abstract control model */
typedef struct _usb_cdc_acm_info {
    bool dtePresent;          /* A flag to indicate whether DTE is present.         */
    uint16_t breakDuration;   /* Length of time in milliseconds of the break signal */
    uint16_t uartState;       /* UART state of the CDC device.                      */
    uint8_t currentInterface; /* Current interface index.                           */
    uint8_t dteStatus;        /* Status of data terminal equipment                  */
    /* Serial state buffer of the CDC device to notify the serial state to host.    */
    uint8_t serialStateBuf[NOTIF_PACKET_SIZE + UART_BITMAP_SIZE];
} usb_cdc_acm_info_t;


/* Line coding of cdc device */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_lineCoding[] = {
    /* E.g. 0x00,0xC2,0x01,0x00 : 0x0001C200 is 115200 bits per second */
    (LINE_CODING_DTERATE >> 0U) & 0x000000FFU,
    (LINE_CODING_DTERATE >> 8U) & 0x000000FFU,
    (LINE_CODING_DTERATE >> 16U) & 0x000000FFU,
    (LINE_CODING_DTERATE >> 24U) & 0x000000FFU,
    LINE_CODING_CHARFORMAT,
    LINE_CODING_PARITYTYPE,
    LINE_CODING_DATABITS
};

/* Abstract state of cdc device */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_abstractState[COMM_FEATURE_DATA_SIZE] = {
    (STATUS_ABSTRACT_STATE >> 0U) & 0x00FFU,
    (STATUS_ABSTRACT_STATE >> 8U) & 0x00FFU
};

/* Country code of cdc device */
USB_DMA_INIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_countryCode[COMM_FEATURE_DATA_SIZE] = {
    (COUNTRY_SETTING >> 0U) & 0x00FFU,
    (COUNTRY_SETTING >> 8U) & 0x00FFU
};

/* CDC ACM information */
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static usb_cdc_acm_info_t s_usbCdcAcmInfo;

/* Data buffer for receiving and sending*/
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_currRecvBuf[HS_CDC_VCOM_BULK_IN_PACKET_SIZE];

USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE)
static uint8_t s_currSendBuf[HS_CDC_VCOM_BULK_OUT_PACKET_SIZE];

static usb_status_t RescheduleRecv(usb_cdc_vcom_struct_t *cdcVcom)
{
    size_t stream_space = xStreamBufferSpacesAvailable(cdcVcom->inputStream);
    size_t endpoint_size = sizeof(s_currRecvBuf);
    usb_status_t error = kStatus_USB_Busy;

    if (stream_space >= endpoint_size)  {
        if ((error = USB_DeviceCdcAcmRecv(cdcVcom->cdcAcmHandle,
                USB_CDC_VCOM_DIC_BULK_OUT_ENDPOINT,
                s_currRecvBuf,
                endpoint_size)) != kStatus_USB_Success)
        {
            PRINTF("[VCOM]: Error: RescheduleRecv FAILED: %u\n", error);
            call_user_cb(cdcVcom, VCOM_WARNING_RESCHEDULE_BUSY);
        }
    }
    return error;
}

static usb_status_t OnSendCompleted(usb_cdc_vcom_struct_t *cdcVcom,
        usb_device_endpoint_callback_message_struct_t *param)
{
    usb_status_t error = kStatus_USB_Success;
    size_t to_send;

    if (cdcVcom->configured) {
        to_send = xStreamBufferReceiveFromISR(cdcVcom->outputStream,
                s_currSendBuf, sizeof(s_currSendBuf), 0);

        if (to_send) {
            if ((error = USB_DeviceCdcAcmSend(cdcVcom->cdcAcmHandle,
                    USB_CDC_VCOM_DIC_BULK_IN_ENDPOINT, s_currSendBuf, to_send))) {
                PRINTF("[VCOM] Error: dropped %u sending bytes", to_send);
                call_user_cb(cdcVcom, VCOM_ERROR_TX_BUFFER_OVERFLOW);
            }
        } else {
            if (param->length > 0) {
                error = USB_DeviceCdcAcmSend(cdcVcom->cdcAcmHandle,
                    USB_CDC_VCOM_DIC_BULK_IN_ENDPOINT, NULL, 0);
            }
        }
    }
    return error;
}

static usb_status_t OnRecvCompleted(usb_cdc_vcom_struct_t *cdcVcom,
        usb_device_endpoint_callback_message_struct_t *param)
{
    usb_status_t error = kStatus_USB_Error;
    if (cdcVcom->startTransactions)
    {
        if (param->length) {
            size_t length = 0;
            length = xStreamBufferSendFromISR(cdcVcom->inputStream, param->buffer, param->length, NULL);
            if (length < param->length) {
                PRINTF("[VCOM] Error: dropped %lu received bytes", param->length - length);
                call_user_cb(cdcVcom, VCOM_ERROR_RX_BUFFER_OVERFLOW);
            }
        }
        error = RescheduleRecv(cdcVcom);
    } else if (param->length == 0xFFFFFFFF) {
        PRINTF("[VCOM] Warning: bus error");
        call_user_cb(cdcVcom, VCOM_ERROR_MALFORMED_USB_PACKET);
    } else {
        PRINTF("[VCOM] Error: missed %lu received bytes", param->length);
        call_user_cb(cdcVcom, VCOM_ERROR_MISSED_INCOMING_DATA);
    }
    return error;
}

usb_status_t VirtualComUSBCallback(uint32_t event, void *param, void *userArg)
{
    usb_status_t error = kStatus_USB_Error;
    uint8_t *uartBitmap;
    usb_cdc_vcom_struct_t *cdcVcom = (usb_cdc_vcom_struct_t*)userArg;
    usb_cdc_acm_info_t *acmInfo = &s_usbCdcAcmInfo;
    usb_device_cdc_acm_request_param_struct_t *acmReqParam;
    usb_device_endpoint_callback_message_struct_t *epCbParam;

    acmReqParam = (usb_device_cdc_acm_request_param_struct_t *)param;
    epCbParam = (usb_device_endpoint_callback_message_struct_t *)param;

    switch (event)
    {
        case kUSB_DeviceCdcEventSendResponse:
            error = OnSendCompleted(cdcVcom, epCbParam);
        break;
        case kUSB_DeviceCdcEventRecvResponse:
        {
            if (cdcVcom->configured) {
                error = OnRecvCompleted(cdcVcom, epCbParam);
            } else {
                    PRINTF("[VCOM] Warning: received frame (%lu bytes) if not configured", epCbParam->length);
                    call_user_cb(cdcVcom, VCOM_WARNING_NOT_CONFIGURED);
            }
        }
        break;
        case kUSB_DeviceCdcEventSerialStateNotif:
            ((usb_device_cdc_acm_struct_t *)cdcVcom->cdcAcmHandle)->hasSentState = 0;
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventSendEncapsulatedCommand:
            break;
        case kUSB_DeviceCdcEventGetEncapsulatedResponse:
            break;
        case kUSB_DeviceCdcEventSetCommFeature:
            if (USB_DEVICE_CDC_FEATURE_ABSTRACT_STATE == acmReqParam->setupValue)
            {
                if (1 == acmReqParam->isSetup)
                {
                    *(acmReqParam->buffer) = s_abstractState;
                }
                else
                {
                    *(acmReqParam->length) = 0;
                }
            }
            else if (USB_DEVICE_CDC_FEATURE_COUNTRY_SETTING == acmReqParam->setupValue)
            {
                if (1 == acmReqParam->isSetup)
                {
                    *(acmReqParam->buffer) = s_countryCode;
                }
                else
                {
                    *(acmReqParam->length) = 0;
                }
            }
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventGetCommFeature:
            if (USB_DEVICE_CDC_FEATURE_ABSTRACT_STATE == acmReqParam->setupValue)
            {
                *(acmReqParam->buffer) = s_abstractState;
                *(acmReqParam->length) = COMM_FEATURE_DATA_SIZE;
            }
            else if (USB_DEVICE_CDC_FEATURE_COUNTRY_SETTING == acmReqParam->setupValue)
            {
                *(acmReqParam->buffer) = s_countryCode;
                *(acmReqParam->length) = COMM_FEATURE_DATA_SIZE;
            }
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventClearCommFeature:
            break;
        case kUSB_DeviceCdcEventGetLineCoding:
            *(acmReqParam->buffer) = s_lineCoding;
            *(acmReqParam->length) = sizeof(s_lineCoding);
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventSetLineCoding:
        {
            if (1 == acmReqParam->isSetup)
            {
                *(acmReqParam->buffer) = s_lineCoding;
            }
            else
            {
                *(acmReqParam->length) = 0;
            }
        }
            error = kStatus_USB_Success;
            break;
        case kUSB_DeviceCdcEventSetControlLineState:
        {
            s_usbCdcAcmInfo.dteStatus = acmReqParam->setupValue;
            /* activate/deactivate Tx carrier */
            if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_CARRIER_ACTIVATION)
            {
                acmInfo->uartState |= USB_DEVICE_CDC_UART_STATE_TX_CARRIER;
            }
            else
            {
                acmInfo->uartState &= (uint16_t)~USB_DEVICE_CDC_UART_STATE_TX_CARRIER;
            }

            /* activate carrier and DTE. Com port of terminal tool running on PC is open now */
            if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE)
            {
                acmInfo->uartState |= USB_DEVICE_CDC_UART_STATE_RX_CARRIER;
            }
            /* Com port of terminal tool running on PC is closed now */
            else
            {
                acmInfo->uartState &= (uint16_t)~USB_DEVICE_CDC_UART_STATE_RX_CARRIER;
            }

            /* Indicates to DCE if DTE is present or not */
            acmInfo->dtePresent = (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE) ? true : false;

            /* Initialize the serial state buffer */
            acmInfo->serialStateBuf[0] = NOTIF_REQUEST_TYPE;                /* bmRequestType */
            acmInfo->serialStateBuf[1] = USB_DEVICE_CDC_NOTIF_SERIAL_STATE; /* bNotification */
            acmInfo->serialStateBuf[2] = 0x00;                              /* wValue */
            acmInfo->serialStateBuf[3] = 0x00;
            acmInfo->serialStateBuf[4] = 0x00; /* wIndex */
            acmInfo->serialStateBuf[5] = 0x00;
            acmInfo->serialStateBuf[6] = UART_BITMAP_SIZE; /* wLength */
            acmInfo->serialStateBuf[7] = 0x00;
            /* Notify to host the line state */
            acmInfo->serialStateBuf[4] = acmReqParam->interfaceIndex;
            /* Lower byte of UART BITMAP */
            uartBitmap = (uint8_t *)&acmInfo->serialStateBuf[NOTIF_PACKET_SIZE + UART_BITMAP_SIZE - 2];
            uartBitmap[0] = acmInfo->uartState & 0xFFu;
            uartBitmap[1] = (acmInfo->uartState >> 8) & 0xFFu;
            if (0 == ((usb_device_cdc_acm_struct_t *)cdcVcom->cdcAcmHandle)->hasSentState)
            {
                error = USB_DeviceCdcAcmSend(cdcVcom->cdcAcmHandle,
                                             USB_CDC_VCOM_CIC_INTERRUPT_IN_ENDPOINT,
                                             acmInfo->serialStateBuf,
                                             sizeof(acmInfo->serialStateBuf));
                if (kStatus_USB_Success != error)
                {
                    PRINTF("kUSB_DeviceCdcEventSetControlLineState error!");
                }
                ((usb_device_cdc_acm_struct_t *)cdcVcom->cdcAcmHandle)->hasSentState = 1;
            }

            /* Update status */
            if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_CARRIER_ACTIVATION)
            {
                /*  To do: CARRIER_ACTIVATED */
            }
            else
            {
                /* To do: CARRIER_DEACTIVATED */
            }
            if (acmInfo->dteStatus & USB_DEVICE_CDC_CONTROL_SIG_BITMAP_DTE_PRESENCE)
            {
                /* DTE_ACTIVATED */
                if (1 == cdcVcom->configured)
                {
                    cdcVcom->startTransactions = 1;
                }
            }
            else
            {
                /* DTE_DEACTIVATED */
                if (1 == cdcVcom->configured)
                {
                    cdcVcom->startTransactions = 0;
                }
            }
        }
        break;
        case kUSB_DeviceCdcEventSendBreak:
            break;
        default:
            break;
    }

    return error;
}


void VirtualComDetached(usb_cdc_vcom_struct_t *cdcVcom)
{
    PRINTF("[VCOM] Info: detached\r\n");
    cdcVcom->configured = false;
    xStreamBufferReceiveFromISR(cdcVcom->outputStream, s_currSendBuf, sizeof(s_currSendBuf), 0);
    call_user_cb(cdcVcom, VCOM_DETACHED);
}

void VirtualComReset(usb_cdc_vcom_struct_t *cdcVcom)
{
    PRINTF("[VCOM] Info: reset\r\n");
    cdcVcom->configured = false;
    xStreamBufferReceiveFromISR(cdcVcom->outputStream, s_currSendBuf, sizeof(s_currSendBuf), 0);
    call_user_cb(cdcVcom, VCOM_RESET);
}

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
usb_status_t VirtualComUSBSetConfiguration(usb_cdc_vcom_struct_t *cdcVcom, uint8_t configure)
{
    if (USB_COMPOSITE_CONFIGURE_INDEX == configure)
    {
        cdcVcom->configured = true;

        PRINTF("[VCOM] Info: configured\r\n");
        /* Schedule buffer for receive */
        RescheduleRecv(cdcVcom);
        call_user_cb(cdcVcom, VCOM_CONFIGURED);
    }
    return kStatus_USB_Success;
}

/*!
 * @brief Virtual COM device initialization function.
 *
 * This function initializes the device with the composite device class information.
 *
 * @param deviceComposite The pointer to the composite device structure.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
usb_status_t VirtualComInit(usb_cdc_vcom_struct_t *cdcVcom, class_handle_t classHandle, userCbFunc callback, void *userArg)
{
    cdcVcom->cdcAcmHandle = classHandle;
    cdcVcom->userCb = callback;
    cdcVcom->userCbArg = userArg;

    if ((cdcVcom->inputStream = xStreamBufferCreate(VCOM_INPUT_STREAM_SIZE, 0)) == NULL) {
        return kStatus_USB_AllocFail;
    }
    if ((cdcVcom->outputStream = xStreamBufferCreate(VCOM_OUTPUT_STREAM_SIZE, 0)) == NULL) {
        return kStatus_USB_AllocFail;
    }
    return kStatus_USB_Success;
}


int VirtualComSend(usb_cdc_vcom_struct_t *cdcVcom, const void* data, size_t length)
{
    size_t result = 0;
    const size_t endpoint_size = sizeof(s_currSendBuf);
    usb_status_t status;
    if (!cdcVcom->configured || !length) {
        return 0;
    }

    taskENTER_CRITICAL();
    if (USB_DeviceClassCdcAcmIsBusy(cdcVcom->cdcAcmHandle, USB_CDC_VCOM_DIC_BULK_IN_ENDPOINT)) {
        result = xStreamBufferSend(cdcVcom->outputStream, data, length, 0);
    } else {
        size_t to_send = (length < endpoint_size) ? length : endpoint_size;
        memcpy(s_currSendBuf, data, to_send);
        status = USB_DeviceCdcAcmSend(cdcVcom->cdcAcmHandle, USB_CDC_VCOM_DIC_BULK_IN_ENDPOINT,
                                     s_currSendBuf, to_send);
        if (status == kStatus_USB_Success) {
            result = to_send;
            if (length > to_send)
                result += xStreamBufferSend(cdcVcom->outputStream, &((uint8_t*)data)[to_send], length - to_send, 0);
        } else {
            result = 0;
        }
    }

    taskEXIT_CRITICAL();
    return result;
}

int VirtualComRecv(usb_cdc_vcom_struct_t *cdcVcom, void* data, size_t length)
{
    size_t received;
    uint8_t busy;
    received = xStreamBufferReceive(cdcVcom->inputStream, data, length, 0);

    // don't care about error code. If pipe is busy, then it will rescheduled in ISR
    if (cdcVcom->configured) {

        taskENTER_CRITICAL();
        busy = USB_DeviceClassCdcAcmIsBusy(cdcVcom->cdcAcmHandle, USB_CDC_VCOM_DIC_BULK_OUT_ENDPOINT);
        if (!busy) {
            RescheduleRecv(cdcVcom);
        }
        taskEXIT_CRITICAL();
    }
    return received;
}

