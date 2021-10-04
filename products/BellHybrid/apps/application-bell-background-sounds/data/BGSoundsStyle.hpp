// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace gui::bgSoundsStyle
{
    inline constexpr auto descriptionFont = style::window::font::verybiglight;
    inline constexpr auto titleFont       = style::window::font::largelight;
    inline constexpr auto timerValueFont  = style::window::font::supersizemelight;
    inline constexpr auto valumeValueFont = style::window::font::supersizemelight;
    namespace progress
    {
        inline constexpr auto bottomDescTopMargin = 15U;
        inline constexpr auto maxProgressValue    = 16;
        inline constexpr auto titleMaxLines       = 2;
        inline constexpr auto titleWidth          = 400U;
    } // namespace progress

    namespace pause
    {
        inline constexpr auto textH = 200U;
    }
} // namespace gui::bgSoundsStyle
