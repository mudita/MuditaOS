// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <SwitchData.hpp>
#include <Units.hpp>

namespace gui
{
    class RelaxationEndedSwitchData : public SwitchData
    {
      public:
        explicit RelaxationEndedSwitchData()
        {
            ignoreCurrentWindowOnStack = true;
        }
    };

    class RelaxationLowBatterySwitchData : public SwitchData
    {
      public:
        explicit RelaxationLowBatterySwitchData(const units::SOC soc) : batteryLevel{soc}
        {
            ignoreCurrentWindowOnStack = true;
        }

        [[nodiscard]] auto getBatteryLevel() const noexcept -> units::SOC
        {
            return batteryLevel;
        }

      private:
        const units::SOC batteryLevel;
    };

} // namespace gui
