#include "usb_device_config.h"
#include "usb.h"
#include "usb_phy.h"
#include "usb_device.h"
#include "usb_device_ch9.h"
#include "usb_device_class.h"
#include "usb_device_mtp.h"

#include "usb_device_descriptor.h"
#include "composite.h"

#include "mtp_responder.h"
#include "mtp_fs.h"

#define PRINTF(...)

USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) uint8_t rx_buffer[512];
USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) uint8_t tx_buffer[512];
USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) uint8_t request[512];
USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) uint8_t response[512];
USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) uint8_t event_response[6];

static mtp_device_info_t dummy_device = {
    .manufacturer = "Mudita",
    .model = "Pure",
    .version = "0.1",
    .serial = "0123456789ABCDEF0123456789ABCDEF",
};

static usb_status_t RescheduleRecv(usb_mtp_struct_t *mtpApp)
{
    // -4 because length of message needs to be stored too
    size_t endpoint_size = 512;
    usb_status_t error = kStatus_USB_Success;
    if (!USB_DeviceClassMtpIsBusy(mtpApp->classHandle, USB_MTP_BULK_OUT_ENDPOINT)
            && !mtpApp->in_reset) {
        size_t available = xMessageBufferSpaceAvailable(mtpApp->inputBox) - 4;
        if (available >= endpoint_size)  {
            error = USB_DeviceClassMtpRecv(mtpApp->classHandle,
                    USB_MTP_BULK_OUT_ENDPOINT,
                    rx_buffer,
                    endpoint_size);
        }
    }
    return error;
}

static usb_status_t ScheduleSend(usb_mtp_struct_t *mtpApp, void *buffer, size_t length)
{
    usb_status_t error = kStatus_USB_Error;
    if (!mtpApp->configured || length > 512 || !length)
        return kStatus_USB_InvalidParameter;

    taskENTER_CRITICAL();

    int is_busy = USB_DeviceClassMtpIsBusy(mtpApp->classHandle, USB_MTP_BULK_IN_ENDPOINT);
    if (!is_busy) {
        memcpy(tx_buffer, buffer, length);
        if ((error = USB_DeviceClassMtpSend(mtpApp->classHandle, USB_MTP_BULK_IN_ENDPOINT, tx_buffer, length)) != kStatus_USB_Success) {
            PRINTF("[MTP] Error: Schedule TX failed. 0x%x\n", error);
        }

    } else {
        if (xMessageBufferSend(mtpApp->outputBox, buffer, length, 0) != length) {
            error = kStatus_USB_Busy;
        } else {
            error = kStatus_USB_Success;
        }
    }
    taskEXIT_CRITICAL();

    return error;
}

static usb_status_t OnConfigurationComplete(usb_mtp_struct_t* mtpApp, void *param)
{
    mtpApp->configured = true;
    return kStatus_USB_Success;
}

static usb_status_t OnIncomingFrame(usb_mtp_struct_t* mtpApp, void *param)
{
    usb_device_endpoint_callback_message_struct_t *epCbParam = (usb_device_endpoint_callback_message_struct_t*) param;

    if (mtpApp->configured) {
        if (epCbParam->length > 0) {
            if (xMessageBufferSendFromISR(mtpApp->inputBox, epCbParam->buffer, epCbParam->length, NULL) != epCbParam->length) {
                PRINTF("[MTP] RX dropped incoming bytes: %u\n", (unsigned int)epCbParam->length);
            }
            //PRINTF("[MTP] RX [%u]: %u\n", xTaskGetTickCount(), epCbParam->length);
        } else {
            PRINTF("[MTP] RX Zero-length\n");
        }
    } else {
        PRINTF("[MTP] RX Unconfigured app\n");
    }

    RescheduleRecv(mtpApp);

    return kStatus_USB_Success;
}

static usb_status_t OnOutgoingFrameSent(usb_mtp_struct_t* mtpApp, void *param)
{
    usb_device_endpoint_callback_message_struct_t *epCbParam = (usb_device_endpoint_callback_message_struct_t*) param;

    if (xMessageBufferIsEmpty(mtpApp->outputBox) != pdTRUE) {
        size_t length = xMessageBufferReceiveFromISR(mtpApp->outputBox, tx_buffer, sizeof(tx_buffer), NULL);
        if (!length) {
            PRINTF("[MTP] Outgoing bytes can't be popped out:\n");
            return kStatus_USB_Error;
        }
        if (USB_DeviceClassMtpSend(mtpApp->classHandle, USB_MTP_BULK_IN_ENDPOINT, tx_buffer, length) != kStatus_USB_Success) {
            PRINTF("[MTP] Dropped outgoing bytes: %u:\n", length);
            return kStatus_USB_Error;
        }

        //PRINTF("[MTP] TX sent: %u\n", length);
    } else {
        //PRINTF("[MTP] TX: Last packet sent (size: %u)\n", epCbParam->length);
    }

    return kStatus_USB_Success;
}

static usb_status_t OnCancelTransaction(usb_mtp_struct_t *mtpApp, void *param)
{
    mtpApp->in_reset = true;
    return kStatus_USB_Success;
}

static usb_status_t OnGetStatus(usb_mtp_struct_t *mtpApp, void *param)
{
    usb_device_control_request_struct_t *request = (usb_device_control_request_struct_t*)param;
    uint16_t status = 0x2001;
    if (mtpApp->in_reset || mtp_responder_data_transaction_open(mtpApp->responder)) {
        status = 0x2019;
    }

    *(uint16_t*)event_response = 0x0004;
    *(uint16_t*)&event_response[2] = status;
    request->buffer = event_response;
    request->length = 4;
    printf("[MTP APP] Control Device Status Response: 0x%04x\n", status);
    return kStatus_USB_Success;
}

usb_status_t MtpUSBCallback(uint32_t event, void *param, void *userArg)
{
    usb_status_t error = kStatus_USB_Error;
    usb_mtp_struct_t* mtpApp = (usb_mtp_struct_t*)userArg;

    switch(event) {
        case kUSB_DeviceMtpEventConfigured:
            error = OnConfigurationComplete(mtpApp, param);
            break;
        case kUSB_DeviceMtpEventSendResponse:
            error = OnOutgoingFrameSent(mtpApp, param);
            break;
        case kUSB_DeviceMtpEventRecvResponse:
            error = OnIncomingFrame(mtpApp, param);
            break;
        case kUSB_DeviceMtpEventCancelTransaction:
            error = OnCancelTransaction(mtpApp, param);
            break;
        case kUSB_DeviceMtpEventRequestDeviceStatus:
            error = OnGetStatus(mtpApp, param);
            break;
        default:
            PRINTF("[MTP APP] Unknown event from device class driver: %d\n", (int)event);
    }

    return error;
}

static void send_response(usb_mtp_struct_t *mtpApp, uint16_t status)
{
    usb_status_t send_status;
    size_t result_len = 0;
    int retries = 3;
    mtp_responder_t *responder = mtpApp->responder;

    mtp_responder_get_response(responder, status, response, &result_len);
    while((send_status = ScheduleSend(mtpApp, response, result_len)) == kStatus_USB_Busy && --retries)
        vTaskDelay(50/portTICK_PERIOD_MS);
    if (send_status != kStatus_USB_Success) {
        PRINTF("[MTP APP]: Transfer failed: 0x%x\n", send_status);
    }
}

static void poll_new_data(usb_mtp_struct_t *mtpApp, size_t *request_len)
{
    do {
        taskENTER_CRITICAL();
        RescheduleRecv(mtpApp);
        taskEXIT_CRITICAL();
        *request_len = xMessageBufferReceive(mtpApp->inputBox, request, sizeof(request), 100/portTICK_PERIOD_MS);
    } while(*request_len == 0 && !mtpApp->in_reset);
}

static void MtpTask(void *handle)
{
    usb_mtp_struct_t* mtpApp = (usb_mtp_struct_t*)handle;
    mtp_responder_t* responder;

    if (!(mtpApp->mtp_fs = mtp_fs_alloc(NULL))) {
        PRINTF("[MTP APP]: MTP FS initialization failed!\n");
        return;
    }

    mtp_responder_init(mtpApp->responder);
    mtp_responder_set_device_info(mtpApp->responder, &dummy_device);
    mtp_responder_set_data_buffer(mtpApp->responder, response, sizeof(response));

    mtp_responder_set_storage(mtpApp->responder,
            0x00010001,
            &simple_fs_api,
            mtpApp->mtp_fs);
    responder = mtpApp->responder;

    PRINTF("[MTP APP] Initialized.\n");

    while(1) {
        // TODO: handle attach and detach
        while(!mtpApp->configured) {
            vTaskDelay(50/portTICK_PERIOD_MS);
        }

        xMessageBufferReset(mtpApp->inputBox);
        xMessageBufferReset(mtpApp->outputBox);
        mtp_responder_transaction_reset(mtpApp->responder);
        printf("MTP reset done\n");

        mtpApp->in_reset = false;

        while(!mtpApp->in_reset) {
            uint16_t status = 0;
            size_t request_len;
            size_t result_len;

            poll_new_data(mtpApp, &request_len);

            if (request_len == 0) {
                PRINTF("[MTP APP]: Expected MTP message. Reset: %s\n", mtpApp->in_reset ? "true" : "false");
                continue;
            }

            // Incoming data transaction open:
            if (mtp_responder_data_transaction_open(responder)) {
                status = mtp_responder_set_data(responder, request, request_len);
                if (status == MTP_RESPONSE_INCOMPLETE_TRANSFER)
                {
                    // This happens with Linux (Nautilus) client. Cancelation procedure
                    // is to just stop sending data in this transaction.
                    // When user triggers another action, client sends GET_OBJECT_INFO
                    // request, which is valid MTP frame and has to be handled.
                    // Don't use timeout here (Windows host can freeze communication
                    // for a while, when assemling file at the end of transacion).
                    PRINTF("[MTP APP]: Incomplete transfer. Expected more data\n");
                    mtp_responder_transaction_reset(mtpApp->responder);
                } else {
                    if (status == MTP_RESPONSE_OK) {
                        PRINTF("[MTP APP]: Incoming transfer complete\n");
                        send_response(mtpApp, status);
                    } else if (status == MTP_RESPONSE_OBJECT_TOO_LARGE) {
                        PRINTF("[MTP APP]: Object is too large\n");
                        send_response(mtpApp, status);
                    }
                    continue;
                }
            }

            status = mtp_responder_handle_request(responder, request, request_len);

            if (status != MTP_RESPONSE_UNDEFINED) {
                while((result_len = mtp_responder_get_data(responder)) && !mtpApp->in_reset) {
                    usb_status_t send_status;

                    if (!xMessageBufferIsEmpty(mtpApp->inputBox)) {
                        // According to spec, initiator can't issue new transacation, before
                        // current one ends. In this case, assume initiator sends new frame
                        // with cancellation request.
                        PRINTF("[MTP APP]: incoming message during data transfer phase. Abort.");
                        mtp_responder_transaction_reset(mtpApp->responder);
                        status = 0;
                        break;
                    }

                    int retries = 5;
                    uint32_t timeout_ms = 5;
                    while((send_status = ScheduleSend(mtpApp, response, result_len)) == kStatus_USB_Busy
                            && --retries
                            && xMessageBufferIsEmpty(mtpApp->outputBox)
                            && !mtpApp->in_reset)
                    {
                        vTaskDelay(timeout_ms/portTICK_PERIOD_MS);
                        timeout_ms *= 2;
                    }

                    if (!retries && !xMessageBufferIsEmpty(mtpApp->outputBox)
                            && !mtpApp->in_reset) {
                        PRINTF("[MTP APP]: Outgoing data canceled (unable to send)\n");
                        mtpApp->in_reset = true;
                        break;
                    }
                }

                if (status && !mtpApp->in_reset) {
                    send_response(mtpApp, status);
                }
            }
        }
    }
}

usb_status_t MtpInit(usb_mtp_struct_t *mtpApp, class_handle_t classHandle)
{
    mtpApp->configured = 0;
    mtpApp->classHandle = classHandle;

    if ((mtpApp->inputBox = xMessageBufferCreate(4*512)) == NULL) {
        return kStatus_USB_AllocFail;
    }

    // SD Card is slower than USB, doesn't need to buffer too much
    if ((mtpApp->outputBox = xMessageBufferCreate(4 + 512)) == NULL) {
        return kStatus_USB_AllocFail;
    }

    if ((mtpApp->responder = mtp_responder_alloc()) == NULL) {
        return kStatus_USB_AllocFail;
    }
    if (xTaskCreate(MtpTask,                  /* pointer to the task */
                    (char const *)"mtp task",       /* task name for kernel awareness debugging */
                    4096 / sizeof(portSTACK_TYPE), /* task stack size */
                    mtpApp,                   /* optional task startup argument */
                    5,                              /* initial priority */
                    NULL             /* optional task handle to create */
                    ) != pdPASS)
    {
        PRINTF("[MTP APP] Create task failed\n");
        return kStatus_USB_AllocFail;
    }
   return kStatus_USB_Success;
}

void MtpDetached(usb_mtp_struct_t *mtpApp)
{
    PRINTF("[MTP APP] MTP detached\n");
    mtpApp->configured = false;
    mtpApp->in_reset = true;
}
