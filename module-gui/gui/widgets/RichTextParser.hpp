// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <map>
#include <utf8/UTF8.hpp>
#include <string>
#include <variant>

namespace gui
{
    class TextDocument;
    class TextFormat;
} // namespace gui

namespace gui::text
{
    constexpr inline auto align      = "align";
    constexpr inline auto center     = "center";
    constexpr inline auto right      = "right";
    constexpr inline auto left       = "left";
    constexpr inline auto color      = "color";
    constexpr inline auto font       = "font";
    constexpr inline auto size       = "size";
    constexpr inline auto weight     = "weight";
    constexpr inline auto regular    = "regular";
    constexpr inline auto bold       = "bold";
    constexpr inline auto light      = "light";
    constexpr inline auto node_text  = "text";
    constexpr inline auto node_br    = "br";
    constexpr inline auto node_p     = "p";
    constexpr inline auto short_bold = "b";
    constexpr inline auto node_token = "token";

    /// Rich text parser utility
    /// supported specifiers
    /// * gui::text::node_text `<text> </text>` attributes:
    ///        * gui::text::align - alignment in Horizontal axis ( gui::text::center, gui::text::left, gui::text::right
    ///        )
    ///        * gui::text::color - Text color from 0 to 15 ( eink black color depth )
    ///        * gui::text::font  - Font to use ( now we have gt_pressura and some dejavu glyphs)
    ///        * gui::text::size  - size of font to use
    ///        * gui::text::weight - weight of font to use, one of: gui::text::bold, gui::text::regular,
    ///        gui::text::light
    /// please mind that selected font must exist on system, othervise closest relative will be selected
    /// * gui::text::node_p `<p> </p>`, gui::text::node_br `<br> </br>` working identical - marking start and end with
    /// newline
    /// * gui::text::short_bold `<b> </b>`  works identical as `<text weight=bold> </text weight=bold>`
    /// * gui::text::node_value `<token>Pattern</token>' replaces /"Pattern/" with respective token value if such is
    /// present it `tokens` argument of `parse` method
    /// @return empty document on error
    class RichTextParser
    {
      public:
        using TokenMap = std::map<std::string, std::variant<int, std::string>>;
        [[nodiscard]] auto parse(const UTF8 &text, TextFormat *base_style, TokenMap &&tokens = TokenMap{})
            -> std::unique_ptr<TextDocument>;
    };
} // namespace gui::text
