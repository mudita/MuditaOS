/*
 *  @file PowerManager.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 12.09.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include <segger/log/log.hpp>

#include "PowerManager.hpp"

namespace sys
{

    PowerManager::PowerManager()
    {
        lowPowerControl = bsp::LowPowerMode::Create().value_or(nullptr);
    }

    PowerManager::~PowerManager()
    {}

    int32_t PowerManager::Switch(const sys::PowerManager::Mode mode)
    {

        int32_t ret = 0;
        switch (mode) {
        case Mode::FullSpeed:
            ret = lowPowerControl->Switch(bsp::LowPowerMode::Mode::FullSpeed);
            break;
        case Mode::LowPowerIdle:
        case Mode::LowPowerRun:
        case Mode::Suspend:
            LOG_FATAL("LowPowerIdle temporary disabled!");
            // Low power is temporary disabled it was breaking i.e. GSM
            // ret = lowPowerControl->Switch(bsp::LowPowerMode::Mode::LowPowerIdle);
            break;
        }

        if (ret == 0) {
            currentPowerMode = mode;
        }
        return ret;
    }

    int32_t PowerManager::PowerOff()
    {
        return lowPowerControl->PowerOff();
    }

} // namespace sys
