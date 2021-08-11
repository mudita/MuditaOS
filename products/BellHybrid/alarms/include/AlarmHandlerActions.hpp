// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-db/Interface/AlarmEventRecord.hpp>

namespace alarms
{

    auto playAlarmSound(const std::string &soundPath) -> bool;
    auto displayAlarmPopup() -> bool;
    auto turnOnFrontlight() -> bool;

} // namespace alarms
