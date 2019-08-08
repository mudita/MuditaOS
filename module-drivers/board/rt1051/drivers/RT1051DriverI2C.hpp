/*
 *  @file RT1051DriverI2C.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_RT1051DRIVERI2C_HPP
#define PUREPHONE_RT1051DRIVERI2C_HPP

#include "drivers/i2c/DriverI2C.hpp"

#include "fsl_common.h"
#include "board.h"
#include "fsl_lpi2c.h"
#include "FreeRTOS.h"
#include "semphr.h"

class RT1051DriverI2C : public DriverI2C {

    RT1051DriverI2C(const DriverParams &params);
    ~RT1051DriverI2C();

    ssize_t Write(const uint8_t *data, const size_t len, const DriverParams &params) override final;

    ssize_t Read(uint8_t *data, const size_t len, const DriverParams &params) override final;

    int32_t Ioctl(const IoctlParams &ioctlParams) override final;

private:

    LPI2C_Type* base;
    SemaphoreHandle_t lock;
};


#endif //PUREPHONE_RT1051DRIVERI2C_HPP
