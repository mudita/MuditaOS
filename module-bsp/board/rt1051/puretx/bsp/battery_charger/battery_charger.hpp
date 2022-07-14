// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MAX77818.hpp"

#include <cstdint>
#include <optional>
#include <fstream>
#include <Units.hpp>

namespace bsp::battery_charger
{
    using Register = std::uint16_t;

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
        ChargerCharging,
        ChargingDone
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

    int init();

    void deinit();

    [[nodiscard]] std::optional<units::SOC> getBatteryLevel();

    void storeBatteryLevelChange(const units::SOC newSocValue);

    batteryRetval getChargeStatus();

    void clearAllChargerIRQs();

    void clearFuelGuageIRQ(std::uint16_t intToClear);

    Register getStatusRegister();

    void checkTemperatureRange();

    std::uint8_t getTopControllerINTSource();

    void setMaxBusCurrent(USBCurrentLimit limit);

    int getVoltageFilteredMeasurement();

    MaxMinVolt getMaxMinVolt();

    void printFuelGaugeInfo();

    bool checkConfigurationFile(std::ifstream &file);

} // namespace bsp::battery_charger
