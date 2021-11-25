// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>

namespace gui::powerNapStyle
{
    inline constexpr auto descriptionFont = style::window::font::largelight;
    inline constexpr auto napTimerFont    = style::window::font::verybig;
    inline constexpr auto napPeriodFont   = style::window::font::supersizeme;
    inline constexpr auto clockFont       = style::window::font::verybiglight;

    namespace progress
    {
        inline constexpr auto bottomDescTopMargin = 20U;
        inline constexpr auto boxesCount          = 16;
    } // namespace progress

    namespace sessionEnd
    {
        inline constexpr auto textH = 200U;
    }
} // namespace gui::powerNapStyle
