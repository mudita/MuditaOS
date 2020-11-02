// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>

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
            ret = lowPowerControl->Switch(bsp::LowPowerMode::Mode::LowPowerIdle);
            break;
        case Mode::LowPowerRun:
            ret = lowPowerControl->Switch(bsp::LowPowerMode::Mode::LowPowerRun);
            break;
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

    int32_t PowerManager::Reboot()
    {
        return lowPowerControl->Reboot();
    }

} // namespace sys
