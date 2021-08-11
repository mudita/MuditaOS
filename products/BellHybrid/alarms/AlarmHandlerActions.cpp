// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmHandlerActions.hpp"

#include <service-audio/AudioServiceAPI.hpp>

namespace alarms
{

    auto playAlarmSound(const std::string &soundPath) -> bool
    {
        // playAlarmSound after it will be implemented [BH-660]
        return true;
    }

    auto displayAlarmPopup() -> bool
    {
        // displayAlarmPopup after it will be implemented [BH-566]
        return true;
    }

    auto turnOnFrontlight() -> bool
    {
        // turnOnFrontlight after it will be implemented [BH-756]
        return true;
    }
} // namespace alarms
