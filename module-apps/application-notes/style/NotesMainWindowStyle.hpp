// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-gui/gui/widgets/Style.hpp>

namespace app::notes::style::main_window
{
    namespace new_note_arrow
    {
        constexpr inline auto X           = 30;
        constexpr inline auto Y           = 62;
        constexpr inline auto ImageSource = "arrow_left";
    } // namespace new_note_arrow

    namespace new_note_image
    {
        constexpr inline auto X           = 48;
        constexpr inline auto Y           = 55;
        constexpr inline auto ImageSource = "cross";
    } // namespace new_note_image

    namespace search_arrow
    {
        constexpr inline auto X           = 480 - 30 - 13;
        constexpr inline auto Y           = 62;
        constexpr inline auto ImageSource = "arrow_right";
    } // namespace search_arrow

    namespace search_image
    {
        constexpr inline auto X           = 480 - 48 - 26;
        constexpr inline auto Y           = 55;
        constexpr inline auto ImageSource = "search";
    } // namespace search_image
} // namespace app::notes::style::main_window
