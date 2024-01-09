// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <SwitchData.hpp>
#include <Units.hpp>
#include <functional>

namespace gui
{
    class BatteryStatusSwitchData : public SwitchData
    {
      public:
        BatteryStatusSwitchData(const units::SOC soc, bool chargeState, bool lowBatteryWarning = false)
            : batteryLevel{soc}, chargeState{chargeState}, lowBatteryWarning{lowBatteryWarning}
        {}

        [[nodiscard]] auto getBatteryLevel() const noexcept -> units::SOC
        {
            return batteryLevel;
        }

        [[nodiscard]] auto isCharging() const noexcept -> bool
        {
            return chargeState;
        }

        [[nodiscard]] auto isLowBatteryWarning() const noexcept -> bool
        {
            return lowBatteryWarning;
        }

      private:
        const units::SOC batteryLevel;
        const bool chargeState;
        const bool lowBatteryWarning;
    };

    class AppsBatteryStatusSwitchData : public BatteryStatusSwitchData
    {
      public:
        AppsBatteryStatusSwitchData(const units::SOC soc, bool chargeState, std::function<void()> returnCallback)
            : BatteryStatusSwitchData{soc, chargeState, true}, returnCallback{returnCallback}
        {}

        [[nodiscard]] auto getReturnCallback() const noexcept -> std::function<void()>
        {
            return returnCallback;
        }

      private:
        std::function<void()> returnCallback;
    };

} // namespace gui
