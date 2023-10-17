// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MAX77818.hpp"

#include <optional>
#include <fstream>
#include <Units.hpp>

namespace bsp::battery_charger
{
    using Register = std::uint16_t;

    enum class BatteryChargerType
    {
        DcdTimeOut = 0x00U, /*!< Dcd detect result is timeout */
        DcdUnknownType,     /*!< Dcd detect result is unknown type */
        DcdError,           /*!< Dcd detect result is error */
        DcdSDP,             /*!< The SDP facility is detected */
        DcdCDP,             /*!< The CDP facility is detected */
        DcdDCP,             /*!< The DCP facility is detected */
    };

    enum class BatteryRetval
    {
        OK,
        ChargerError,
        ChargerNotCharging,
        ChargerPluggedNotCharging,
        ChargerCharging,
        ChargingDone
    };

    enum class TemperatureRanges
    {
        Cold,
        Cdeg1to15,
        Cdeg15to35,
        Cdeg35to45,
        Hot,
        Unknown
    };

    enum class TopControllerIRQsource
    {
        CHGR_INT = (1 << 0),
        FG_INT   = (1 << 1),
        SYS_INT  = (1 << 2)
    };

    enum class BatteryINTBSource
    {
        SOCOnePercentChange = (1 << 7),
        minVAlert           = (1 << 8),
        minTemp             = (1 << 9),
        minSOCAlert         = (1 << 10),
        maxTemp             = (1 << 13),
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
    [[nodiscard]] TemperatureRanges getTemperatureRange();

    void storeBatteryLevelChange(units::SOC newSocValue);

    BatteryRetval getChargeStatus();

    void clearAllChargerIRQs();
    void clearFuelGaugeIRQ(std::uint16_t intToClear);

    Register getStatusRegister();

    void checkTemperatureRange();

    std::optional<std::uint8_t> getTopControllerINTSource();

    void setMaxBusCurrent(USBCurrentLimit limit);

    std::optional<int> getVoltageFilteredMeasurement();
    std::optional<MaxMinVolt> getMaxMinVolt();

    void printFuelGaugeInfo();

    bool isChargerPlugged();
} // namespace bsp::battery_charger
