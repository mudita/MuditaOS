/*
 * Copyright (c) 2020, OnPlick
 * All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"

#if USB_DEVICE_CONFIG_MTP
#include "usb_device_mtp.h"
#include "usb_device_descriptor.h"

#define USB_MTP_ENTER_CRITICAL() \
    USB_OSA_SR_ALLOC();              \
    USB_OSA_ENTER_CRITICAL()

#define USB_MTP_EXIT_CRITICAL() USB_OSA_EXIT_CRITICAL()

USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE)
    usb_device_mtp_struct_t g_mtpHandle[USB_DEVICE_CONFIG_MTP];

static usb_status_t USB_DeviceClassMtpAllocateHandle(usb_device_mtp_struct_t **handle)
{
    uint32_t count;
    for (count = 0; count < USB_DEVICE_CONFIG_MTP; count++)
    {
        if (NULL == g_mtpHandle[count].handle)
        {
            *handle = &g_mtpHandle[count];
            return kStatus_USB_Success;
        }
    }

    return kStatus_USB_Busy;
}

static usb_status_t USB_DeviceClassMtpFreeHandle(usb_device_mtp_struct_t *handle)
{
    handle->handle        = NULL;
    handle->configStruct  = NULL;
    handle->configuration = 0;
    handle->alternate     = 0;
    return kStatus_USB_Success;
}

static usb_status_t USB_DeviceClassMtpInterruptIn(usb_device_handle handle,
                                      usb_device_endpoint_callback_message_struct_t *message,
                                      void *callbackParam)
{
    usb_status_t error = kStatus_USB_Error;
    usb_echo("%s\r\n", __FUNCTION__);
    return error;
}

static usb_status_t USB_DeviceClassMtpBulkIn(usb_device_handle handle,
                                 usb_device_endpoint_callback_message_struct_t *message,
                                 void *callbackParam)
{
    usb_status_t error = kStatus_USB_Error;
    usb_device_mtp_struct_t *mtpHandle;
    mtpHandle = (usb_device_mtp_struct_t *)callbackParam;

    if (!mtpHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    mtpHandle->bulkIn.isBusy = 0;

    if ((NULL != mtpHandle->configStruct) && (mtpHandle->configStruct->classCallback))
    {
        /*classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        error = mtpHandle->configStruct->classCallback(kUSB_DeviceMtpEventSendResponse,
                                                       message,
                                                       mtpHandle->configStruct->classCalbackArg);
    }
    return error;
}

static usb_status_t USB_DeviceClassMtpBulkOut(usb_device_handle handle,
                                  usb_device_endpoint_callback_message_struct_t *message,
                                  void *callbackParam)
{
    usb_status_t error = kStatus_USB_Error;

    usb_device_mtp_struct_t *mtpHandle;
    mtpHandle = (usb_device_mtp_struct_t *)callbackParam;

    if (!mtpHandle)
    {
        return kStatus_USB_InvalidHandle;
    }

    mtpHandle->bulkOut.isBusy = 0;

    if ((NULL != mtpHandle->configStruct) && (mtpHandle->configStruct->classCallback))
    {
        /*classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        error = mtpHandle->configStruct->classCallback(
                kUSB_DeviceMtpEventRecvResponse, message, mtpHandle->configStruct->classCalbackArg);
    }
    return error;
}

usb_status_t USB_DeviceClassMtpEndpointsInit(usb_device_mtp_struct_t *mtpHandle)
{
    usb_status_t error = kStatus_USB_Error;

    usb_device_interface_list_t *interfaceList;
    usb_device_interface_struct_t *interface = NULL;

    if (!mtpHandle)
    {
        return error;
    }

    /* return error when configuration is invalid (0 or more than the configuration number) */
    if ((mtpHandle->configuration == 0U) ||
        (mtpHandle->configuration > mtpHandle->configStruct->classInfomation->configurations))
    {
        return error;
    }

    interfaceList = &mtpHandle->configStruct->classInfomation->interfaceList[mtpHandle->configuration - 1];

    for (uint32_t count = 0; count < interfaceList->count; count++)
    {
        if (USB_MTP_DEVICE_CLASS == interfaceList->interfaces[count].classCode)
        {
            for (uint32_t index = 0; index < interfaceList->interfaces[count].count; index++)
            {
                if (interfaceList->interfaces[count].interface[index].alternateSetting == mtpHandle->alternate)
                {
                    interface = &interfaceList->interfaces[count].interface[index];
                    break;
                }
            }
            mtpHandle->interfaceNumber = interfaceList->interfaces[count].interfaceNumber;
            break;
        }
    }

    if (!interface)
    {
        return error;
    }

    mtpHandle->interfaceHandle = interface;

    for (uint32_t count = 0; count < interface->endpointList.count; count++)
    {
        usb_device_endpoint_init_struct_t epInitStruct;
        usb_device_endpoint_callback_struct_t epCallback;
        epInitStruct.zlt             = 0;
        epInitStruct.interval        = interface->endpointList.endpoint[count].interval;
        epInitStruct.endpointAddress = interface->endpointList.endpoint[count].endpointAddress;
        epInitStruct.maxPacketSize   = interface->endpointList.endpoint[count].maxPacketSize;
        epInitStruct.transferType    = interface->endpointList.endpoint[count].transferType;

        if ((USB_IN == ((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                        USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)) &&
            (USB_ENDPOINT_BULK == epInitStruct.transferType))
        {
            mtpHandle->bulkIn.ep     = (epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            mtpHandle->bulkIn.isBusy = 0;
            mtpHandle->bulkIn.pipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
            mtpHandle->bulkIn.pipeStall      = 0U;
            mtpHandle->bulkIn.pipeDataLen    = 0U;
            epCallback.callbackFn               = USB_DeviceClassMtpBulkIn;
        }
        else if ((USB_OUT == ((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                              USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)) &&
                 (USB_ENDPOINT_BULK == epInitStruct.transferType))
        {
            mtpHandle->bulkOut.ep     = (epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            mtpHandle->bulkOut.isBusy = 0;
            mtpHandle->bulkOut.pipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
            mtpHandle->bulkOut.pipeStall      = 0U;
            mtpHandle->bulkOut.pipeDataLen    = 0U;
            epCallback.callbackFn                = USB_DeviceClassMtpBulkOut;
        }
        else if ((USB_IN == ((epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_MASK) >>
                        USB_DESCRIPTOR_ENDPOINT_ADDRESS_DIRECTION_SHIFT)) &&
            (USB_ENDPOINT_INTERRUPT == epInitStruct.transferType))
        {
            mtpHandle->interruptIn.ep = (epInitStruct.endpointAddress & USB_DESCRIPTOR_ENDPOINT_ADDRESS_NUMBER_MASK);
            mtpHandle->interruptIn.isBusy         = 0;
            mtpHandle->interruptIn.pipeDataBuffer = (uint8_t *)USB_UNINITIALIZED_VAL_32;
            mtpHandle->interruptIn.pipeStall      = 0U;
            mtpHandle->interruptIn.pipeDataLen    = 0U;
            epCallback.callbackFn                    = USB_DeviceClassMtpInterruptIn;
        }

        epCallback.callbackParam = mtpHandle;

        error = USB_DeviceInitEndpoint(mtpHandle->handle, &epInitStruct, &epCallback);
    }
    return error;
}

usb_status_t USB_DeviceClassMtpEndpointsDeinit(usb_device_mtp_struct_t *mtpHandle)
{
    usb_status_t error = kStatus_USB_Error;

    if (!mtpHandle->interfaceHandle)
    {
        return error;
    }
    for (uint32_t count = 0; count < mtpHandle->interfaceHandle->endpointList.count; count++)
    {
        error = USB_DeviceDeinitEndpoint(mtpHandle->handle, mtpHandle->interfaceHandle->endpointList.endpoint[count].endpointAddress);
    }
    mtpHandle->interfaceHandle = NULL;
    return error;
}

static usb_status_t handler_DeviceClassEventDeviceReset(usb_device_mtp_struct_t *mtp_device, void *param)
{
    (void)param;
    mtp_device->configuration = 0;
    usb_echo("[MTP] reset\r\n");
    return kStatus_USB_Success;
}

static usb_status_t handler_DeviceClassEventSetConfiguration(usb_device_mtp_struct_t *mtp, void *param)
{
    uint8_t cfg_id = *(uint8_t *)param;
    usb_status_t error = kStatus_USB_Error;

    if (!mtp || !mtp->configStruct)
    {
        return kStatus_USB_InvalidHandle;
    }
    if (cfg_id == mtp->configuration)
    {
        return kStatus_USB_Error;
    }

    if ((error = USB_DeviceClassMtpEndpointsDeinit(mtp)))
    {
        usb_echo("[MTP]: endpoints deinit failed: %d\r\n", error);
    }

    mtp->configuration = cfg_id;
    mtp->alternate     = 0;

    if ((error = USB_DeviceClassMtpEndpointsInit(mtp)))
    {
        usb_echo("[MTP]: endpoints init failed: %d\r\n", error);
        return error;
    }

    error = mtp->configStruct->classCallback(kUSB_DeviceMtpEventConfigured, NULL, mtp->configStruct->classCalbackArg);

    return error;
}

static usb_status_t handler_DeviceClassEventSetInterface(usb_device_mtp_struct_t *mtp, void *param)
{
    usb_echo("[MTP]: %s not implemented\r\n", __FUNCTION__);
    return kStatus_USB_InvalidParameter;
}
static usb_status_t handler_DeviceClassEventSetEndpointHalt(usb_device_mtp_struct_t *mtp, void *param)
{
    usb_status_t error = kStatus_USB_Error;
    usb_echo("[MTP]: %s not implemented\r\n", __FUNCTION__);
    return error;
}
static usb_status_t handler_DeviceClassEventClearEndpointHalt(usb_device_mtp_struct_t *mtp, void *param)
{
    usb_status_t error = kStatus_USB_Error;
    usb_echo("[MTP]: %s not implemented\r\n", __FUNCTION__);
    return error;
}

static usb_status_t handler_DeviceClassEventClassRequest(usb_device_mtp_struct_t *mtp, void *param)
{
    usb_status_t error = kStatus_USB_Error;

    if (param)
    {
        usb_device_control_request_struct_t *controlRequest = (usb_device_control_request_struct_t *)param;

        if ((controlRequest->setup->wIndex & 0xFFU) != mtp->interfaceNumber)
        {
            return kStatus_USB_Error;
        }

        usb_device_control_request_struct_t *request = (usb_device_control_request_struct_t*)param;
        if (request->setup) {
            usb_echo("[MTP] Control request: [bmRequestType = 0x%x, bRequest = 0x%x, wValue = 0x%x, wIndex = 0x%x, wLength = 0x%x]\n",
                    request->setup->bmRequestType,
                    request->setup->bRequest,
                    request->setup->wValue,
                    request->setup->wIndex,
                    request->setup->wLength);

        }


        switch(controlRequest->setup->bRequest) {
            case MTP_CLASS_REQUEST_CANCEL:
                error = mtp->configStruct->classCallback(kUSB_DeviceMtpEventCancelTransaction, controlRequest, mtp->configStruct->classCalbackArg);
                break;
            case MTP_CLASS_REQUEST_DEVICE_STATUS:
                error = mtp->configStruct->classCallback(kUSB_DeviceMtpEventRequestDeviceStatus, controlRequest, mtp->configStruct->classCalbackArg);
                break;
            case MTP_CLASS_REQUEST_RESET:
                usb_echo("[MTP]: MTP_CLASS_REQUEST_RESET not implemented\n");
                break;
            default:
                usb_echo("[MTP]: Unknown class request: 0x%x\n", controlRequest->setup->bRequest);
        }
    }
    return error;
}

usb_status_t USB_DeviceClassMtpEvent(void *handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Error;
    usb_device_mtp_struct_t *mtpHandle;

    if ((!param) || (!handle))
    {
        return kStatus_USB_InvalidHandle;
    }

    mtpHandle = (usb_device_mtp_struct_t *)handle;

    switch (event)
    {
        case kUSB_DeviceClassEventDeviceReset:
            error = handler_DeviceClassEventDeviceReset(mtpHandle, param);
            break;
        case kUSB_DeviceClassEventSetConfiguration:
            error = handler_DeviceClassEventSetConfiguration(mtpHandle, param);
            break;
        case kUSB_DeviceClassEventSetInterface:
            error = handler_DeviceClassEventSetInterface(mtpHandle, param);
            break;
        case kUSB_DeviceClassEventSetEndpointHalt:
            error = handler_DeviceClassEventSetEndpointHalt(mtpHandle, param);
            break;
        case kUSB_DeviceClassEventClearEndpointHalt:
            error = handler_DeviceClassEventClearEndpointHalt(mtpHandle, param);
            break;
        case kUSB_DeviceClassEventClassRequest:
            error = handler_DeviceClassEventClassRequest(mtpHandle, param);
            break;
        default:
            error = kStatus_USB_InvalidParameter;
            break;
    }
    return error;
}

usb_status_t USB_DeviceClassMtpInit(uint8_t controllerId,
                                  usb_device_class_config_struct_t *config,
                                  class_handle_t *handle)
{
    usb_status_t error = kStatus_USB_Error;
    usb_device_mtp_struct_t *mtpHandle;

    error = USB_DeviceClassMtpAllocateHandle(&mtpHandle);
    if (kStatus_USB_Success != error)
    {
        return error;
    }

    error = USB_DeviceClassGetDeviceHandle(controllerId, &mtpHandle->handle);
    if (kStatus_USB_Success != error)
    {
        return error;
    }

    if (!mtpHandle->handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    mtpHandle->configStruct  = config;
    mtpHandle->configuration = 0;
    mtpHandle->alternate     = 0xFF;

    if (KOSA_StatusSuccess != OSA_MutexCreate(&(mtpHandle->bulkIn.mutex)))
    {
#ifdef DEBUG
        usb_echo("mutex create error!");
#endif
    }
    if (KOSA_StatusSuccess != OSA_MutexCreate(&(mtpHandle->bulkOut.mutex)))
    {
#ifdef DEBUG
        usb_echo("mutex create error!");
#endif
    }
    if (KOSA_StatusSuccess != OSA_MutexCreate(&(mtpHandle->interruptIn.mutex)))
    {
#ifdef DEBUG
        usb_echo("mutex create error!");
#endif
    }
    *handle = (class_handle_t)mtpHandle;
    return error;
}

usb_status_t USB_DeviceClassMtpDeinit(class_handle_t handle)
{
    usb_status_t error = kStatus_USB_Error;
    usb_device_mtp_struct_t *mtpHandle;

    mtpHandle = (usb_device_mtp_struct_t *)handle;

    if (!mtpHandle)
    {
        return kStatus_USB_InvalidHandle;
    }
    if (KOSA_StatusSuccess != OSA_MutexDestroy((mtpHandle->bulkIn.mutex)))
    {
#ifdef DEBUG
        usb_echo("mutex destroy error!");
#endif
    }
    if (KOSA_StatusSuccess != OSA_MutexDestroy((mtpHandle->bulkOut.mutex)))
    {
#ifdef DEBUG
        usb_echo("mutex destroy error!");
#endif
    }
    if (KOSA_StatusSuccess != OSA_MutexDestroy((mtpHandle->interruptIn.mutex)))
    {
#ifdef DEBUG
        usb_echo("mutex destroy error!");
#endif
    }
    error = USB_DeviceClassMtpEndpointsDeinit(mtpHandle);
    USB_DeviceClassMtpFreeHandle(mtpHandle);
    return error;
}

usb_status_t USB_DeviceClassMtpSend(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length)
{
    usb_status_t error = kStatus_USB_Error;
    usb_device_mtp_struct_t *mtpHandle;
    usb_device_mtp_pipe_t *mtpPipe = NULL;

    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    mtpHandle = (usb_device_mtp_struct_t *)handle;

    if (mtpHandle->bulkIn.ep == ep)
    {
        mtpPipe = &(mtpHandle->bulkIn);
    }
    else if (mtpHandle->interruptIn.ep == ep)
    {
        mtpPipe = &(mtpHandle->interruptIn);
    }
    else
    {
    }

    if (NULL != mtpPipe)
    {
        if (1U == mtpPipe->isBusy)
        {
            return kStatus_USB_Busy;
        }
        mtpPipe->isBusy = 1U;

        if (mtpPipe->pipeStall)
        {
            mtpPipe->pipeDataBuffer = buffer;
            mtpPipe->pipeDataLen    = length;
            return kStatus_USB_Success;
        }

        error = USB_DeviceSendRequest(mtpHandle->handle, ep, buffer, length);
        if (kStatus_USB_Success != error)
        {
            mtpPipe->isBusy = 0U;
        }
    }
    return error;
}

usb_status_t USB_DeviceClassMtpRecv(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length)
{
    usb_status_t error = kStatus_USB_Error;
    usb_device_mtp_struct_t *mtpHandle;


    if (!handle)
    {
        return kStatus_USB_InvalidHandle;
    }
    mtpHandle = (usb_device_mtp_struct_t *)handle;

    if (1U == mtpHandle->bulkOut.isBusy)
    {
        return kStatus_USB_Busy;
    }
    mtpHandle->bulkOut.isBusy = 1U;

    if (mtpHandle->bulkOut.pipeStall)
    {
        mtpHandle->bulkOut.pipeDataBuffer = buffer;
        mtpHandle->bulkOut.pipeDataLen    = length;
        return kStatus_USB_Success;
    }

    error = USB_DeviceRecvRequest(mtpHandle->handle, ep, buffer, length);
    if (kStatus_USB_Success != error)
    {
        mtpHandle->bulkOut.isBusy = 0U;
    }
    return error;
}

int USB_DeviceClassMtpIsBusy(class_handle_t handle, uint8_t ep)
{
    usb_device_mtp_struct_t *mtpHandle;
    mtpHandle = (usb_device_mtp_struct_t *)handle;

    if (ep == mtpHandle->bulkIn.ep)
        return mtpHandle->bulkIn.isBusy;
    if (ep == mtpHandle->interruptIn.ep)
        return mtpHandle->interruptIn.isBusy;
    if (ep == mtpHandle->bulkOut.ep)
        return mtpHandle->bulkOut.isBusy;
    return 1;
}

#endif /* USB_DEVICE_CONFIG_MTP */
