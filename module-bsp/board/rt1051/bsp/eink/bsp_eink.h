/*
 *  @file bsp_eink.h
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 7 sie 2018
 *  @brief
 *  @copyright Copyright (C) 2018 mudita.com
 *  @details
 */

#ifndef EINK_BSP_EINK_H_
#define EINK_BSP_EINK_H_

#include <stdint.h>
#include "fsl_common.h"
#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    SPI_AUTOMATIC_CS,
    SPI_MANUAL_CS
}eink_spi_cs_config_e;

typedef enum
{
	BSP_Eink_CS_Clr = 0,
	BSP_Eink_CS_Set = 1
}bsp_eink_cs_ctrl_t;

typedef void (*bsp_eink_BusyEvent)(void);


#define BSP_EINK_TRANSFER_TIMEOUT_MS        1000

status_t BSP_EinkInit(bsp_eink_BusyEvent event);
void BSP_EinkWriteCS(bsp_eink_cs_ctrl_t ctrl);
uint8_t BSP_EinkWaitUntilDisplayBusy(uint32_t timeout);
void BSP_EinkResetDisplayController(void);

status_t BSP_EinkChangeSpiFrequency(uint32_t frequencyHz);
status_t BSP_EinkWriteData(void* txBuffer, uint32_t len, eink_spi_cs_config_e cs);
status_t BSP_EinkReadData(void* rxBuffer, uint32_t len, eink_spi_cs_config_e cs);

BaseType_t BSP_EinkBusyPinStateChangeHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* EINK_BSP_EINK_H_ */
