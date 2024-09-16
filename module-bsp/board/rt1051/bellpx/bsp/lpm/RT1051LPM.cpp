// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "RT1051LPM.hpp"
#include "WfiController.hpp"

namespace bsp
{
    void RT1051LPM::EnableDcdcPowerSaveMode()
    {}

    void RT1051LPM::DisableDcdcPowerSaveMode()
    {}

    void RT1051LPM::AllowEnteringWfiMode()
    {
        allowEnteringWfiMode();
    }

    void RT1051LPM::BlockEnteringWfiMode()
    {
        blockEnteringWfiMode();
    }

    std::uint32_t RT1051LPM::EnterWfiModeIfAllowed()
    {
        return enterWfiModeIfAllowed();
    }

    std::uint32_t RT1051LPM::GetLastTimeSpentInWfi()
    {
        return getLastTimeSpentInWfi();
    }
} // namespace bsp
