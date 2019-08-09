/*
 *  @file BoardDefinitions.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_BOARDDEFINITIONS_HPP
#define PUREPHONE_BOARDDEFINITIONS_HPP

#include "drivers/i2c/DriverI2C.hpp"

enum class BoardDefinitions{
    AUDIOCODEC_I2C_BAUDRATE = 100000,
    AUDIOCODEC_I2C = static_cast<int >(drivers::I2CInstances ::I2C2),

    KEYBOARD_I2C_BAUDRATE = AUDIOCODEC_I2C_BAUDRATE,
    KEYBOARD_I2C = AUDIOCODEC_I2C,

    BATTERY_CHARGER_I2C_BAUDRATE = AUDIOCODEC_I2C_BAUDRATE,
    BATTERY_CHARGER_I2C = AUDIOCODEC_I2C,
};

#endif //PUREPHONE_BOARDDEFINITIONS_HPP
