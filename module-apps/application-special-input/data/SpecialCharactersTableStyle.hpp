// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>
#include <utf8/UTF8.hpp>
#include <functional>
#include <array>

namespace specialInputStyle
{
    enum class CharactersType
    {
        Emoji,
        SpecialCharacters
    };

    constexpr inline auto symbol_for_newline  = U'␤';
    constexpr inline std::array special_chars = {
        U'.', U',', U'\'', U'?', U'!', U'"', U'-', U'(', U')', U'@', U'/', U':', U'_', U';', symbol_for_newline,
        U'+', U'&', U'%',  U'*', U'<', U'>', U'=', U'£', U'€', U'$', U'[', U']', U'{', U'}', U'\'',
        U'^', U'~', U'`',  U'į', U'§', U'…', U'#', U'|', U'÷', U'·', U'°', U'¿', U'¡', U'ą', U'à',
        U'á', U'ä', U'â',  U'ć', U'ç', U'ę', U'é', U'è', U'ê', U'ë', U'î', U'ï', U'í', U'ł', U'ń',
        U'ñ', U'ó', U'ô',  U'ö', U'ś', U'û', U'ú', U'ù', U'ü', U'ÿ', U'ż', U'ź', U'ß'};

    constexpr inline std::array emojis = {U'😁', U'😂', U'😃', U'😄', U'😅', U'😆', U'😉', U'😊', U'😋', U'😌', U'😍',
                                          U'😏', U'😒', U'😓', U'😔', U'😖', U'😘', U'😚', U'😜', U'😝', U'😼'};

    namespace specialInputListView
    {
        constexpr uint32_t x = style::window::default_left_margin;
        constexpr uint32_t y = style::window::default_vertical_pos;
        constexpr uint32_t w = style::listview::body_width_with_scroll;
        constexpr uint32_t h = style::window::default_body_height;
    } // namespace specialInputListView

    namespace specialCharacterTableWidget
    {
        constexpr auto window_grid_w  = style::window::default_body_width;
        constexpr auto window_grid_h  = style::window::default_body_height;
        constexpr auto char_grid_w    = 60;
        constexpr auto char_grid_h    = 60;
        constexpr auto char_label_w   = 50;
        constexpr auto char_label_h   = 50;
        constexpr auto char_newline_x = 15;
        constexpr auto char_newline_y = 15;
    } // namespace specialCharacterTableWidget

} // namespace specialInputStyle
