// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <bsp/lpm/RT1051LPMCommon.hpp>

namespace bsp
{
    class RT1051LPM : public RT1051LPMCommon
    {
      public:
        void EnableDcdcPowerSaveMode() final;
        void DisableDcdcPowerSaveMode() final;

        void AllowEnteringWfiMode() final;
        void BlockEnteringWfiMode() final;
        std::uint32_t EnterWfiModeIfAllowed() final;
        std::uint32_t GetLastTimeSpentInWfi() final;
    };
} // namespace bsp
