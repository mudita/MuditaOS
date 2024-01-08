// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>

namespace bsp
{
    void allowEnteringWfiMode();
    void blockEnteringWfiMode();
    std::uint32_t enterWfiModeIfAllowed();
    std::uint32_t getLastTimeSpentInWfi();
} // namespace bsp
