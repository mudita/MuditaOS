// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace gui::bgSoundsStyle
{
    inline constexpr auto descriptionFont = style::window::font::largelight;
    inline constexpr auto timerValueFont  = style::window::font::supersizemelight;
    namespace progress
    {
        inline constexpr auto bottomDescTopMargin = 20U;
        inline constexpr auto boxesCount          = 16;
    } // namespace progress

    namespace pause
    {
        inline constexpr auto textH = 200U;
    }
} // namespace gui::bgSoundsStyle
