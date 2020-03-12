/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
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

#include "fsl_sdmmc_event.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Card detect event. */
static volatile uint32_t g_eventCardDetect;

/*! @brief transfer complete event. */
static volatile uint32_t g_eventTransferComplete;

/*! @brief Time variable unites as milliseconds. */
volatile uint32_t g_eventTimeMilliseconds;

static QueueHandle_t event_sync = NULL;
static SemaphoreHandle_t guard  = NULL;

uint32_t cntnotify = 0;
uint32_t cntwait   = 0;
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Create event structure.
 *
 * @return true or false.
 */
bool SDMMCEVENT_Create(void)
{
    event_sync = xQueueCreate(1, sizeof(uint32_t));

    if (event_sync == NULL) {
        return false;
    }

    guard = xSemaphoreCreateBinary();
    if (guard == NULL) {
        return false;
    }
    xSemaphoreGive(guard);

    return true;
}

/*!
 * @brief Wait specified time in milliseconds(blocking).
 *
 * This function tries to receive sync event for timeoutMilliseconds.
 * @param timeoutMilliseconds	Wait time in milliseconds.
 * @return true or false in case of error or timeout.
 */
bool SDMMCEVENT_Wait(uint32_t timeoutMilliseconds)
{
    assert(event_sync);

    if (timeoutMilliseconds) {
        cntwait++;
        status_t status = 0;

        BaseType_t ret = xQueueReceive(event_sync, &status, timeoutMilliseconds);

        if ((ret == pdPASS) && (status == kStatus_Success)) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}

/*!
 * @brief Try acquire lock guard
 *
 * This function tries to lock for timeoutMilliseconds.
 * @param timeoutMilliseconds	Wait time in milliseconds.
 * @return true or false in case of error or timeout.
 */
bool SDMMCEVENT_Lock(uint32_t timeoutMilliseconds)
{
    assert(guard);

    if (timeoutMilliseconds) {
        status_t status = 0;

        BaseType_t ret = xSemaphoreTake(guard, timeoutMilliseconds);

        if ((ret == pdPASS) && (status == kStatus_Success)) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}

/*!
 * @brief Release lock
 *
 *
 * This function unlocks lock
 * @param timeoutMilliseconds	Wait time in milliseconds.
 * @return true or false in case of error or timeout.
 */
bool SDMMCEVENT_Unlock(void)
{
    assert(guard);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(guard, &xHigherPriorityTaskWoken);
    return true;
}

/*!
 * @brief Send sync event.
 *
 * This function sends notification event, it can be used only from interrupt routine.
 * @param status Transfer complete status, success or failure.
 * @return Information if context switch from IRQ is needed.
 */
BaseType_t SDMMCEVENT_Notify(status_t status)
{
    cntnotify++;
    assert(event_sync);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    BaseType_t ret = xQueueSendFromISR(event_sync, &status, &xHigherPriorityTaskWoken);
    if (ret != pdPASS) {
        assert(0);
    }

    /* If xHigherPriorityTaskWoken was set to pdTRUE inside
    xStreamBufferSendFromISR() then a task that has a priority above the
    priority of the currently executing task was unblocked and a context
    switch should be performed to ensure the ISR returns to the unblocked
    task.  In most FreeRTOS ports this is done by simply passing
    xHigherPriorityTaskWoken into taskYIELD_FROM_ISR(), which will test the
    variables value, and perform the context switch if necessary.  Check the
    documentation for the port in use for port specific instructions. */

    return xHigherPriorityTaskWoken;
}

/*!
 * @brief Clear sync structures.
 *
 * This function clears notification structures.
 * @return No return.
 */
void SDMMCEVENT_Delete()
{
    uint32_t dummy = 0;

    if (xQueueReceive(event_sync, &dummy, 0) != errQUEUE_EMPTY) {
        assert(0);
    }
}

/*!
 * @brief Generic delay function
 *
 * This function blocks until specified time.
 * @param milliseconds Block time in milliseconds.
 * @return No return.
 */
void SDMMCEVENT_Delay(uint32_t milliseconds)
{
    vTaskDelay(pdMS_TO_TICKS(milliseconds));
}
