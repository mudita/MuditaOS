// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <SwitchData.hpp>

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
} // namespace gui
