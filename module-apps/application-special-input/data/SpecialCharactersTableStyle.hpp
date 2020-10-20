// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Style.hpp>
#include <utf8/UTF8.hpp>
#include <functional>
#include <vector>

namespace specialInputStyle
{
    enum class CharactersType
    {
        Emoji,
        SpecialCharacters
    };

    const inline char32_t symbol_for_newline         = U'␤';
    const inline std::vector<char32_t> special_chars = {
        U'.', U',', U'\'', U'?', U'!', U'"', U'-', U'(', U')', U'@', U'/', U':', U'_', U';', symbol_for_newline,
        U'+', U'&', U'%',  U'*', U'<', U'>', U'=', U'£', U'€', U'$', U'[', U']', U'{', U'}', U'\'',
        U'^', U'~', U'`',  U'į', U'§', U'…', U'#', U'|', U'÷', U'·', U'°', U'¿', U'¡', U'ą', U'à',
        U'á', U'ä', U'â',  U'ć', U'ç', U'ę', U'é', U'è', U'ê', U'ë', U'î', U'ï', U'í', U'ł', U'ń',
        U'ñ', U'ó', U'ô',  U'ö', U'ś', U'û', U'ú', U'ù', U'ü', U'ÿ', U'ż', U'ź', U'ß'};

    const inline std::vector<char32_t> emojis = {U'😁', U'😂', U'😃', U'😄', U'😅', U'😆', U'😉', U'😊', U'😋', U'😌', U'😍',
                                                 U'😏', U'😒', U'😓', U'😔', U'😖', U'😘', U'😚', U'😜', U'😝', U'😼'};

    namespace specialInputListView
    {
        constexpr uint32_t x = style::window::default_left_margin;
        constexpr uint32_t y = style::header::height;
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
