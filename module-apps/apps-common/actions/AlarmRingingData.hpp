// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <SwitchData.hpp>

namespace app::actions
{
    class AlarmRingingData : public gui::SwitchData
    {
      public:
        AlarmRingingData() : SwitchData()
        {}
    };
} // namespace app::actions
