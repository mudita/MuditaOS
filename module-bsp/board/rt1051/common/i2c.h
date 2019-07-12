/*
 *  @file bsp_i2c.h
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23 Oct 2018
 *  @brief I2C BSP driver.
 *  @copyright Copyright (C) 2018 mudita.com
 *  @details
 */

#ifndef BSP_COMMON_BSP_I2C_H_
#define BSP_COMMON_BSP_I2C_H_

#include "fsl_common.h"
#include "board.h"
#include "fsl_lpi2c.h"
#include "FreeRTOS.h"
#include "semphr.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
	LPI2C_Type* base;
	SemaphoreHandle_t lock;
}bsp_i2c_inst_t;

status_t bsp_i2c_Init(bsp_i2c_inst_t* inst, uint32_t clkSrc_Hz);

status_t bsp_i2c_Send(bsp_i2c_inst_t* inst, uint8_t deviceAddress, uint32_t subAddress,
				uint32_t subAddressSize, uint8_t *txBuff,uint8_t size);

status_t bsp_i2c_Receive(bsp_i2c_inst_t* inst, uint8_t deviceAddress, uint32_t subAddress,
				uint32_t subAddressSize, uint8_t *rxBuff,uint8_t size);

status_t bsp_i2c_ModifyReg(bsp_i2c_inst_t* inst, uint8_t deviceAddress, uint32_t subAddress,
				uint32_t subAddressSize, uint16_t mask, uint8_t setClr,uint8_t size);


bsp_i2c_inst_t* BOARD_GetI2CInstance(void);

#ifdef __cplusplus
}
#endif


#endif /* BSP_COMMON_BSP_I2C_H_ */
