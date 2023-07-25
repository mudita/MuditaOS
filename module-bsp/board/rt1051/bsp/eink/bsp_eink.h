﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef EINK_BSP_EINK_H_
#define EINK_BSP_EINK_H_

#include <cstdint>
#include "fsl_common.h"
#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        SPI_AUTOMATIC_CS,
        SPI_MANUAL_CS
    } eink_spi_cs_config_e;

    typedef enum
    {
        BSP_Eink_CS_Clr = 0,
        BSP_Eink_CS_Set = 1
    } bsp_eink_cs_ctrl_t;

    typedef void (*bsp_eink_BusyEvent)(void);

    inline constexpr auto BSP_EinkBusyTimeout = 3000U;

    status_t BSP_EinkInit();
    void BSP_EinkDeinit(void);

    void BSP_EinkLogicPowerOn();
    void BSP_EinkLogicPowerOff();

    void BSP_EinkWriteCS(bsp_eink_cs_ctrl_t ctrl);
    std::uint8_t BSP_EinkWaitUntilDisplayBusy(std::uint32_t timeout);
    void BSP_EinkResetDisplayController(void);

    status_t BSP_EinkChangeSpiFrequency(uint32_t frequencyHz);
    status_t BSP_EinkWriteData(void *txBuffer, uint32_t len, eink_spi_cs_config_e cs);
    status_t BSP_EinkReadData(void *rxBuffer, uint32_t len, eink_spi_cs_config_e cs);

    BaseType_t BSP_EinkBusyPinStateChangeHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* EINK_BSP_EINK_H_ */
