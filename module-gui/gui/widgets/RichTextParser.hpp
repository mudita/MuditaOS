// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <map>
#include <utf8/UTF8.hpp>
#include <string>

namespace gui
{
    class TextDocument;
    class TextFormat;
} // namespace gui

namespace gui::text
{
    inline const auto align     = "align";
    inline const auto center    = "center";
    inline const auto right     = "right";
    inline const auto left      = "left";
    inline const auto color     = "color";
    inline const auto font      = "font";
    inline const auto size      = "size";
    inline const auto weight    = "weight";
    inline const auto regular   = "regular";
    inline const auto bold      = "bold";
    inline const auto light     = "light";
    inline const auto node_text = "text";
    inline const auto node_br   = "br";
    inline const auto node_p    = "p";
    inline const auto short_bold = "b";
    inline const auto node_value = "val";

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
    /// * gui::text::node_value `<val>Pattern</val>' replaces /"Pattern/" with respective numeric value if such is
    /// present it `values` argument of `parse` method
    /// @return empty document on error
    class RichTextParser
    {
      public:
        auto parse(const UTF8 &text, TextFormat *base_style) -> std::unique_ptr<TextDocument>;
        auto parse(const UTF8 &text, TextFormat *base_style, std::map<std::string, int> &&values)
            -> std::unique_ptr<TextDocument>;
    };
} // namespace gui::text
