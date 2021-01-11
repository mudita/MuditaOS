// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace style::desktop
{

    namespace notifications
    {
        constexpr auto SpanSize     = 8;
        constexpr auto DigitSize    = 16;
        constexpr auto IconWidth    = 35;
        constexpr auto TextMaxWidth = 250;

        constexpr auto X     = 0;
        constexpr auto Y     = 284;
        constexpr auto Width = style::window_width;

    }; // namespace notifications

    namespace timeLabel
    {
        constexpr auto X     = 0;
        constexpr auto Y     = 106;
        constexpr auto Width = style::window_width;
        constexpr auto Hight = 96;

    } // namespace timeLabel

    namespace dayLabel
    {
        constexpr auto X     = 0;
        constexpr auto Y     = 204;
        constexpr auto Width = style::window_width;
        constexpr auto Hight = 51;

    } // namespace dayLabel

} // namespace style::desktop
