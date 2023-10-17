// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
