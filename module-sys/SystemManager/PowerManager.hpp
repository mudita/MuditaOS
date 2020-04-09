/*
 *  @file PowerManager.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 12.09.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

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
