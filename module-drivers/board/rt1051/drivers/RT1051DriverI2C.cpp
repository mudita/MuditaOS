/*
 *  @file RT1051DriverI2C.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "RT1051DriverI2C.hpp"
#include "log/log.hpp"
#include "../board.h"

namespace drivers {


    RT1051DriverI2C::RT1051DriverI2C(const I2CInstances inst,const DriverI2CParams &params){

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
        LPI2C_MasterInit(BOARD_KEYBOARD_I2C_BASEADDR, &lpi2cConfig, BOARD_KEYBOARD_I2C_CLOCK_FREQ);
    }

    RT1051DriverI2C::~RT1051DriverI2C() {
        LPI2C_MasterDeinit(BOARD_KEYBOARD_I2C_BASEADDR);
    }

    ssize_t RT1051DriverI2C::Write(const uint8_t *data, const size_t len, const DriverI2CParams &params) {

    }

    ssize_t RT1051DriverI2C::Read(uint8_t *data, const size_t len, const DriverI2CParams &params) {

    }

}