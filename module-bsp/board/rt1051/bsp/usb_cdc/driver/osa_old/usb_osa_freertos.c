/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_device_registers.h"
#include "stdint.h"
#include "usb.h"
#include "usb_osa.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define MSEC_TO_TICK(msec) ((1000L + ((uint32_t)configTICK_RATE_HZ * (uint32_t)(msec - 1U))) / 1000L)
#define TICKS_TO_MSEC(tick) ((tick)*1000uL / (uint32_t)configTICK_RATE_HZ)

/* FreeRTOS Event status structure */
typedef struct _usb_osa_event_struct
{
    EventGroupHandle_t handle; /* The event handle */
    uint32_t flag;             /* Event flags, includes auto clear flag */
} usb_osa_event_struct_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void *USB_OsaMemoryAllocate(uint32_t length)
{
    void *p = (void *)pvPortMalloc(length);
    uint8_t *temp = (uint8_t *)p;
    if (p)
    {
        for (uint32_t count = 0U; count < length; count++)
        {
            temp[count] = 0U;
        }
    }
    return p;
}

void USB_OsaMemoryFree(void *p)
{
    vPortFree(p);
}
void USB_OsaEnterCritical(uint8_t *sr)
{
    if (__get_IPSR())
    {
        *sr = portSET_INTERRUPT_MASK_FROM_ISR();
    }
    else
    {
        portENTER_CRITICAL();
    }
}

void USB_OsaExitCritical(uint8_t sr)
{
    if (__get_IPSR())
    {
        portCLEAR_INTERRUPT_MASK_FROM_ISR(sr);
    }
    else
    {
        portEXIT_CRITICAL();
    }
}

usb_osa_status_t USB_OsaEventCreate(usb_osa_event_handle *handle, uint32_t flag)
{
    usb_osa_event_struct_t *event;

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }
    event = (usb_osa_event_struct_t *)USB_OsaMemoryAllocate(sizeof(usb_osa_event_struct_t));
    if (NULL == event)
    {
        return kStatus_USB_OSA_Error;
    }

    event->handle = xEventGroupCreate();
    if (NULL == event->handle)
    {
        USB_OsaMemoryFree(event);
        return kStatus_USB_OSA_Error;
    }
    event->flag = flag;
    *handle = event;
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaEventDestroy(usb_osa_event_handle handle)
{
    usb_osa_event_struct_t *event = (usb_osa_event_struct_t *)handle;
    if (handle)
    {
        if (event->handle)
        {
            vEventGroupDelete(event->handle);
        }
        USB_OsaMemoryFree(handle);
        return kStatus_USB_OSA_Success;
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaEventSet(usb_osa_event_handle handle, uint32_t bitMask)
{
    usb_osa_event_struct_t *event = (usb_osa_event_struct_t *)handle;
    portBASE_TYPE taskToWake = pdFALSE;
    if (handle)
    {
        if (__get_IPSR())
        {
            if (pdPASS == xEventGroupSetBitsFromISR(event->handle, (EventBits_t)bitMask, &taskToWake))
            {
                portYIELD_FROM_ISR(taskToWake);
            }
        }
        else
        {
            xEventGroupSetBits(event->handle, (EventBits_t)bitMask);
        }
        return kStatus_USB_OSA_Success;
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaEventWait(usb_osa_event_handle handle, uint32_t bitMask, uint32_t flag, uint32_t timeout, uint32_t *bitSet)
{
    usb_osa_event_struct_t *event = (usb_osa_event_struct_t *)handle;
    BaseType_t autoClear;
    EventBits_t bits;

    if (handle)
    {
        if (!timeout)
        {
            timeout = portMAX_DELAY;
        }
        else
        {
            timeout = MSEC_TO_TICK(timeout);
        }

        if (event->flag)
        {
            autoClear = pdTRUE;
        }
        else
        {
            autoClear = pdFALSE;
        }

        bits = xEventGroupWaitBits(event->handle, (EventBits_t)bitMask, autoClear, (BaseType_t)flag, timeout);

        if (bitSet)
        {
            *bitSet = bits & ((EventBits_t)bitMask);
            if (*bitSet)
            {
                return kStatus_USB_OSA_Success;
            }
            return kStatus_USB_OSA_TimeOut;
        }
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaEventCheck(usb_osa_event_handle handle, uint32_t bitMask, uint32_t *bitSet)
{
    usb_osa_event_struct_t *event = (usb_osa_event_struct_t *)handle;
    EventBits_t bits;

    if (handle)
    {
        if (__get_IPSR())
        {
            bits = xEventGroupGetBitsFromISR(event->handle);
        }
        else
        {
            bits = xEventGroupGetBits(event->handle);
        }
        bits = (bits & bitMask);
        if (bits)
        {
            if (bitSet)
            {
                *bitSet = bits & ((EventBits_t)bitMask);
            }
            return kStatus_USB_OSA_Success;
        }
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaEventClear(usb_osa_event_handle handle, uint32_t bitMask)
{
    EventBits_t ev;
    usb_osa_event_struct_t *event = (usb_osa_event_struct_t *)handle;

    if (handle)
    {
        if (__get_IPSR())
        {
            xEventGroupClearBitsFromISR(event->handle, (EventBits_t)bitMask);
        }
        else
        {
            ev = xEventGroupClearBits(event->handle, (EventBits_t)bitMask);
            if (ev == 0)
            {
                return kStatus_USB_OSA_Error;
            }
        }
        return kStatus_USB_OSA_Success;
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaSemCreate(usb_osa_sem_handle *handle, uint32_t count)
{
    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

    *handle = (usb_osa_sem_handle)xSemaphoreCreateCounting(0xFFU, count);
    if (NULL == (*handle))
    {
        return kStatus_USB_OSA_Error;
    }
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaSemDestroy(usb_osa_sem_handle handle)
{
    if (handle)
    {
        vSemaphoreDelete(handle);
        return kStatus_USB_OSA_Success;
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaSemPost(usb_osa_sem_handle handle)
{
    xSemaphoreHandle sem = (xSemaphoreHandle)handle;
    portBASE_TYPE taskToWake = pdFALSE;

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

    if (__get_IPSR())
    {
        if (pdPASS == xSemaphoreGiveFromISR(sem, &taskToWake))
        {
            portYIELD_FROM_ISR(taskToWake);
            return kStatus_USB_OSA_Success;
        }
    }
    else
    {
        if (pdTRUE == xSemaphoreGive(sem))
        {
            return kStatus_USB_OSA_Success;
        }
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaSemWait(usb_osa_sem_handle handle, uint32_t timeout)
{
    xSemaphoreHandle sem = (xSemaphoreHandle)handle;

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

    if (!timeout)
    {
        timeout = portMAX_DELAY;
    }
    else
    {
        timeout = MSEC_TO_TICK(timeout);
    }

    if (pdFALSE == xSemaphoreTake(sem, timeout))
    {
        return kStatus_USB_OSA_TimeOut;
    }
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMutexCreate(usb_osa_mutex_handle *handle)
{
    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }
    *handle = (usb_osa_mutex_handle)xSemaphoreCreateRecursiveMutex();
    if (NULL == *handle)
    {
        return kStatus_USB_OSA_Error;
    }
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMutexDestroy(usb_osa_mutex_handle handle)
{
    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }
    vSemaphoreDelete((xSemaphoreHandle)handle);
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMutexLock(usb_osa_mutex_handle handle)
{
    xSemaphoreHandle mutex = (xSemaphoreHandle)handle;

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

    if (xSemaphoreTakeRecursive(mutex, portMAX_DELAY) == pdFALSE)
    {
        return kStatus_USB_OSA_TimeOut;
    }

    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMutexUnlock(usb_osa_mutex_handle handle)
{
    xSemaphoreHandle mutex = (xSemaphoreHandle)handle;

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

    if (xSemaphoreGiveRecursive(mutex) == pdFALSE)
    {
        return kStatus_USB_OSA_Error;
    }
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqCreate(usb_osa_msgq_handle *handle, uint32_t count, uint32_t size)
{
    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }
    *handle = xQueueCreate(count, size * sizeof(uint32_t));
    if (NULL == *handle)
    {
        return kStatus_USB_OSA_Error;
    }

    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqDestroy(usb_osa_msgq_handle handle)
{
    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }
    vQueueDelete((xQueueHandle)handle);
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqSend(usb_osa_msgq_handle handle, void *msg)
{
    xQueueHandle msgq = (xQueueHandle)handle;
    portBASE_TYPE taskToWake = pdFALSE;

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

    if (__get_IPSR())
    {
        if (pdPASS == xQueueSendToBackFromISR(msgq, msg, &taskToWake))
        {
            portYIELD_FROM_ISR(taskToWake);
            return kStatus_USB_OSA_Success;
        }
    }
    else
    {
        if (pdPASS == xQueueSendToBack(msgq, msg, 0U))
        {
            return kStatus_USB_OSA_Success;
        }
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaMsgqRecv(usb_osa_msgq_handle handle, void *msg, uint32_t timeout)
{
    xQueueHandle msgq = (xQueueHandle)handle;

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

    if (!timeout)
    {
        timeout = portMAX_DELAY;
    }
    else
    {
        timeout = MSEC_TO_TICK(timeout);
    }
    if (pdPASS != xQueueReceive(msgq, msg, timeout))
    {
        return kStatus_USB_OSA_TimeOut;
    }
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqCheck(usb_osa_msgq_handle handle, void *msg)
{
    xQueueHandle msgq = (xQueueHandle)handle;

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

    if (uxQueueMessagesWaiting(msgq))
    {
        if (pdPASS == xQueueReceive(msgq, msg, 1U))
        {
            return kStatus_USB_OSA_Success;
        }
    }

    return kStatus_USB_OSA_Error;
}
