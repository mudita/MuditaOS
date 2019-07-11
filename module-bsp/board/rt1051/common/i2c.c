/*
 *  @file bsp_i2c.c
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23 Oct 2018
 *  @brief I2C BSP driver.
 *  @copyright Copyright (C) 2018 mudita.com
 *  @details
 */

#include "i2c.h"

static void BOARD_LPI2C_Init(LPI2C_Type *base, uint32_t clkSrc_Hz);

static status_t BOARD_LPI2C_Send(LPI2C_Type *base, uint8_t deviceAddress, uint32_t subAddress,
                uint8_t subAddressSize, uint8_t *txBuff, uint8_t txBuffSize);

static status_t BOARD_LPI2C_Receive(LPI2C_Type *base, uint8_t deviceAddress, uint32_t subAddress,
                uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);

status_t BOARD_LPI2C_SendSCCB(LPI2C_Type *base,
                              uint8_t deviceAddress,
                              uint32_t subAddress,
                              uint8_t subAddressSize,
                              uint8_t *txBuff,
                              uint8_t txBuffSize);

status_t BOARD_LPI2C_ReceiveSCCB(LPI2C_Type *base,
                                 uint8_t deviceAddress,
                                 uint32_t subAddress,
                                 uint8_t subAddressSize,
                                 uint8_t *rxBuff,
                                 uint8_t rxBuffSize);


static bsp_i2c_inst_t i2c_gen = {.base = BOARD_KEYBOARD_I2C_BASEADDR,.lock = NULL};

bsp_i2c_inst_t* BOARD_GetI2CInstance(void)
{
    return &i2c_gen;
}

status_t bsp_i2c_Init(bsp_i2c_inst_t* inst, uint32_t clkSrc_Hz)
{
	inst->lock = NULL;

	inst->lock = xSemaphoreCreateMutex();
	if(inst->lock == NULL){
		return kStatus_Fail;
	}
	BOARD_LPI2C_Init(BOARD_KEYBOARD_I2C_BASEADDR, BOARD_KEYBOARD_I2C_CLOCK_FREQ);

	return kStatus_Success;
}


status_t bsp_i2c_Send(bsp_i2c_inst_t* inst, uint8_t deviceAddress, uint32_t subAddress,
				uint32_t subAddressSize, uint8_t *txBuff,uint8_t size)
{
	status_t ret = kStatus_Success;
	if(xSemaphoreTake(inst->lock,100) != pdPASS){
		return kStatus_Timeout;
	}

	ret = BOARD_LPI2C_Send(inst->base,deviceAddress,subAddress,subAddressSize,(uint8_t*)txBuff,size);

	xSemaphoreGive(inst->lock);

	return ret;
}


status_t bsp_i2c_Receive(bsp_i2c_inst_t* inst, uint8_t deviceAddress, uint32_t subAddress,
				uint32_t subAddressSize, uint8_t *rxBuff,uint8_t size)
{
	status_t ret = kStatus_Success;
	if(xSemaphoreTake(inst->lock,100) != pdPASS){
		return kStatus_Timeout;
	}

	ret = BOARD_LPI2C_Receive(inst->base,deviceAddress,subAddress,subAddressSize,(uint8_t*)rxBuff,size);

	xSemaphoreGive(inst->lock);

	return ret;
}

status_t bsp_i2c_ModifyReg(bsp_i2c_inst_t* inst, uint8_t deviceAddress, uint32_t subAddress,
				uint32_t subAddressSize, uint16_t mask, uint8_t setClr,uint8_t size)
{

	status_t ret = kStatus_Success;
	if(xSemaphoreTake(inst->lock,100) != pdPASS){
		return kStatus_Timeout;
	}

	uint16_t rx = 0;
	ret |= BOARD_LPI2C_Receive(inst->base, deviceAddress, subAddress, size, (uint8_t*)&rx,
			size);

	// Set specified bits
	if(setClr == 1){
		rx |=mask;
	}
	else{
		rx &=~mask;
	}

    ret |= BOARD_LPI2C_Send(inst->base, deviceAddress, subAddress, size, (uint8_t*)&rx,
    		size);

    xSemaphoreGive(inst->lock);

    return ret;
}



static void BOARD_LPI2C_Init(LPI2C_Type *base, uint32_t clkSrc_Hz)
{
    lpi2c_master_config_t lpi2cConfig = {0};

    /*
     * lpi2cConfig.debugEnable = false;
     * lpi2cConfig.ignoreAck = false;
     * lpi2cConfig.pinConfig = kLPI2C_2PinOpenDrain;
     * lpi2cConfig.baudRate_Hz = 100000U;
     * lpi2cConfig.busIdleTimeout_ns = 0;
     * lpi2cConfig.pinLowTimeout_ns = 0;
     * lpi2cConfig.sdaGlitchFilterWidth_ns = 0;
     * lpi2cConfig.sclGlitchFilterWidth_ns = 0;
     */
    LPI2C_MasterGetDefaultConfig(&lpi2cConfig);
    LPI2C_MasterInit(base, &lpi2cConfig, clkSrc_Hz);
}

static status_t BOARD_LPI2C_Send(LPI2C_Type *base, uint8_t deviceAddress, uint32_t subAddress,
                uint8_t subAddressSize, uint8_t *txBuff, uint8_t txBuffSize)
{
    status_t reVal;

    /* Send master blocking data to slave */
    reVal = LPI2C_MasterStart(base, deviceAddress, kLPI2C_Write);
    if (kStatus_Success == reVal)
    {
        while (LPI2C_MasterGetStatusFlags(base) & kLPI2C_MasterNackDetectFlag)
        {
        }

        reVal = LPI2C_MasterSend(base, &subAddress, subAddressSize);
        if (reVal != kStatus_Success)
        {
            return reVal;
        }

        reVal = LPI2C_MasterSend(base, txBuff, txBuffSize);
        if (reVal != kStatus_Success)
        {
            return reVal;
        }

        reVal = LPI2C_MasterStop(base);
        if (reVal != kStatus_Success)
        {
            return reVal;
        }
    }

    return reVal;
}

static status_t BOARD_LPI2C_Receive(LPI2C_Type *base, uint8_t deviceAddress, uint32_t subAddress,
                uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize)
{
    status_t reVal;

    reVal = LPI2C_MasterStart(base, deviceAddress, kLPI2C_Write);
    if (kStatus_Success == reVal)
    {
        while (LPI2C_MasterGetStatusFlags(base) & kLPI2C_MasterNackDetectFlag)
        {
        }

        reVal = LPI2C_MasterSend(base, &subAddress, subAddressSize);
        if (reVal != kStatus_Success)
        {
            return reVal;
        }

        reVal = LPI2C_MasterRepeatedStart(base, deviceAddress, kLPI2C_Read);

        if (reVal != kStatus_Success)
        {
            return reVal;
        }

        reVal = LPI2C_MasterReceive(base, rxBuff, rxBuffSize);
        if (reVal != kStatus_Success)
        {
            return reVal;
        }

        reVal = LPI2C_MasterStop(base);
        if (reVal != kStatus_Success)
        {
            return reVal;
        }
    }
    return reVal;
}

status_t BOARD_LPI2C_SendSCCB(LPI2C_Type *base,
                              uint8_t deviceAddress,
                              uint32_t subAddress,
                              uint8_t subAddressSize,
                              uint8_t *txBuff,
                              uint8_t txBuffSize)
{
    return BOARD_LPI2C_Send(base, deviceAddress, subAddress, subAddressSize, txBuff, txBuffSize);
}

status_t BOARD_LPI2C_ReceiveSCCB(LPI2C_Type *base,
                                 uint8_t deviceAddress,
                                 uint32_t subAddress,
                                 uint8_t subAddressSize,
                                 uint8_t *rxBuff,
                                 uint8_t rxBuffSize)
{
    status_t reVal;

    reVal = LPI2C_MasterStart(base, deviceAddress, kLPI2C_Write);
    if (kStatus_Success == reVal)
    {
        while (LPI2C_MasterGetStatusFlags(base) & kLPI2C_MasterNackDetectFlag)
        {
        }

        reVal = LPI2C_MasterSend(base, &subAddress, subAddressSize);
        if (reVal != kStatus_Success)
        {
            return reVal;
        }

        /* SCCB does not support LPI2C repeat start, must stop then start. */
        reVal = LPI2C_MasterStop(base);

        if (reVal != kStatus_Success)
        {
            return reVal;
        }

        reVal = LPI2C_MasterStart(base, deviceAddress, kLPI2C_Read);

        if (reVal != kStatus_Success)
        {
            return reVal;
        }

        reVal = LPI2C_MasterReceive(base, rxBuff, rxBuffSize);
        if (reVal != kStatus_Success)
        {
            return reVal;
        }

        reVal = LPI2C_MasterStop(base);
        if (reVal != kStatus_Success)
        {
            return reVal;
        }
    }
    return reVal;
}
