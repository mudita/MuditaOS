/*
 * Copyright (c) 2020, OnPlick
 * All rights reserved.
 */
#ifndef _USB_DEVICE_MTP_H_
#define _USB_DEVICE_MTP_H_

#define MTP_CLASS_REQUEST_CANCEL (0x64)
#define MTP_CLASS_REQUEST_RESET (0x66)
#define MTP_CLASS_REQUEST_DEVICE_STATUS (0x67)


typedef enum _usb_device_mtp_event
{
    kUSB_DeviceMtpEventConfigured = 0x01,
    kUSB_DeviceMtpEventSendResponse,            /*!< This event indicates the bulk send transfer is complete or cancelled etc. */
    kUSB_DeviceMtpEventRecvResponse,            /*!< This event indicates the bulk receive transfer is complete or cancelled etc.. */
    kUSB_DeviceMtpEventCancelTransaction,
    kUSB_DeviceMtpEventRequestDeviceStatus,
    kUSB_DeviceMtpEventRequestDeviceReset,
} usb_device_mtp_event_t;

typedef struct _usb_device_mtp_pipe
{
    osa_mutex_handle_t mutex; /*!< The mutex of the pipe. */
    uint8_t *pipeDataBuffer;      /*!< pipe data buffer backup when stall */
    uint32_t pipeDataLen;         /*!< pipe data length backup when stall  */
    uint8_t pipeStall;            /*!< pipe is stall  */
    uint8_t ep;                 /*!< The endpoint number of the pipe. */
    uint8_t isBusy;             /*!< 1: The pipe is transferring packet, 0: The pipe is idle. */
} usb_device_mtp_pipe_t;

typedef struct _usb_device_mtp_struct
{
    usb_device_handle handle;
    usb_device_class_config_struct_t *configStruct;
    usb_device_interface_struct_t *interfaceHandle;
    usb_device_mtp_pipe_t bulkIn;
    usb_device_mtp_pipe_t bulkOut;
    usb_device_mtp_pipe_t interruptIn;
    uint8_t configuration;
    uint8_t interfaceNumber;
    uint8_t alternate;
    uint8_t hasSentState;
} usb_device_mtp_struct_t;

#if defined(__cplusplus)
extern "C" {
#endif

extern usb_status_t USB_DeviceClassMtpInit(uint8_t controllerId,
                                      usb_device_class_config_struct_t *config,
                                      class_handle_t *handle);
extern usb_status_t USB_DeviceClassMtpDeinit(class_handle_t handle);
extern usb_status_t USB_DeviceClassMtpEvent(void *handle, uint32_t event, void *param);
extern usb_status_t USB_DeviceClassMtpSend(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length);
extern usb_status_t USB_DeviceClassMtpRecv(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length);
extern int USB_DeviceClassMtpIsBusy(class_handle_t handle, uint8_t ep);

#if defined(__cplusplus)
}
#endif

#endif /* _USB_DEVICE_MTP_H_ */
