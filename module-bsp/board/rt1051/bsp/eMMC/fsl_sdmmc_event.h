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
#ifndef _FSL_SDMMC_EVENT_H_
#define _FSL_SDMMC_EVENT_H_

#include "fsl_common.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C"
{
#endif

    /*!
     * @name Event Function
     * @{
     */

    /*!
     * @brief Initialize timer to implement wait event timeout.
     */
    void SDMMCEVENT_InitTimer(void);

    /* Callback function for SDHC */

    /*!
     * @brief Create event.
     * @param eventType The event type
     * @retval true Create event successfully.
     * @retval false Create event failed.
     */
    bool SDMMCEVENT_Create(void);

    /*!
     * @brief Wait event.
     *
     * @param eventType The event type
     * @param timeoutMilliseconds Timeout time in milliseconds.
     * @retval true Wait event successfully.
     * @retval false Wait event failed.
     */
    bool SDMMCEVENT_Wait(uint32_t timeoutMilliseconds);

    /*!
     * @brief Notify event.
     * @param status Transfer status
     * @retval true Notify event successfully.
     * @retval false Notify event failed.
     */
    BaseType_t SDMMCEVENT_Notify(status_t status);

    /*!
     * @brief Delete event.
     * @param eventType The event type
     */
    void SDMMCEVENT_Delete(void);

    /*!
     * @brief sdmmc delay.
     * @param milliseconds time to delay
     */
    void SDMMCEVENT_Delay(uint32_t milliseconds);

    bool SDMMCEVENT_Unlock(void);
    bool SDMMCEVENT_Lock(uint32_t timeoutMilliseconds);
    /* @} */

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_SDMMC_EVENT_H_*/
