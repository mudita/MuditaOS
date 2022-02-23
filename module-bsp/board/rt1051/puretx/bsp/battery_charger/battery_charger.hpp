// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MAX77818.hpp"

#include <cstdint>
extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
}

#include <array>

namespace bsp::battery_charger
{
    using StateOfCharge = std::uint8_t;

    enum class batteryChargerType
    {
        DcdTimeOut = 0x00U, /*!< Dcd detect result is timeout */
        DcdUnknownType,     /*!< Dcd detect result is unknown type */
        DcdError,           /*!< Dcd detect result is error*/
        DcdSDP,             /*!< The SDP facility is detected */
        DcdCDP,             /*!< The CDP facility is detected */
        DcdDCP,             /*!< The DCP facility is detected */
    };

    enum class batteryRetval
    {
        OK,
        ChargerError,
        ChargerNotCharging,
        ChargerCharging
    };

    enum class batteryIRQSource
    {
        INTB  = 0x01,
        INOKB = 0x02
    };

    enum class topControllerIRQsource
    {
        CHGR_INT = (1 << 0),
        FG_INT   = (1 << 1),
        SYS_INT  = (1 << 2)
    };

    enum class batteryINTBSource
    {
        maxTemp             = 1 << 13,
        minSOCAlert         = 1 << 10,
        minTemp             = 1 << 9,
        minVAlert           = 1 << 8,
        SOCOnePercentChange = 1 << 7,
        all                 = 0xFFFF
    };

    struct MaxMinVolt
    {
        unsigned minMilliVolt{0};
        unsigned maxMilliVolt{0};
    };

    int init(xQueueHandle irqQueueHandle, xQueueHandle dcdQueueHandle);

    void deinit();

    StateOfCharge getBatteryLevel();

    void evaluateBatteryLevelChange(const std::uint16_t currentLevel, const std::uint16_t updatedLevel);

    bool getChargeStatus();

    void clearAllChargerIRQs();

    void clearFuelGuageIRQ(std::uint16_t intToClear);

    std::uint16_t getStatusRegister();

    void checkTemperatureRange();

    std::uint8_t getTopControllerINTSource();

    void setMaxBusCurrent(USBCurrentLimit limit);

    void actionIfChargerUnplugged();

    int getVoltageFilteredMeasurement();

    MaxMinVolt getMaxMinVolt();

    void printFuelGaugeInfo();

    BaseType_t INTB_IRQHandler();

    BaseType_t INOKB_IRQHandler();

    void USBChargerDetectedHandler(std::uint8_t detectedType);

} // namespace bsp::battery_charger
