// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_POWERMANAGER_HPP
#define PUREPHONE_POWERMANAGER_HPP

#include <functional>

#include "bsp/lpm/bsp_lpm.hpp"

namespace sys
{

    class PowerManager
    {

      public:
        enum class Mode
        {
            FullSpeed,
            LowPowerRun,
            LowPowerIdle,
            Suspend

        };

        PowerManager();
        ~PowerManager();

        int32_t Switch(const Mode mode);
        int32_t PowerOff();
        int32_t Reboot();

        void SetCpuFrequency(const bsp::LowPowerMode::CpuFrequency freq);

        Mode GetCurrentMode()
        {
            return currentPowerMode;
        }

      private:
        std::unique_ptr<bsp::LowPowerMode> lowPowerControl;

        Mode currentPowerMode = Mode ::FullSpeed;
    };

} // namespace sys

#endif // PUREPHONE_POWERMANAGER_HPP
