// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ImageBox.hpp>
#include <TextFixedSize.hpp>

namespace gui

{
    namespace imageBoxWithText
    {
        inline constexpr auto wh                 = 100;
        inline constexpr auto text_margin_top    = 11;
        inline constexpr auto text_margin_bottom = 5;
        inline constexpr auto font               = style::window::font::verysmall;
        inline constexpr auto fontBold           = style::window::font::verysmallbold;
    } // namespace imageBoxWithText

    class ImageBoxWithText : public ImageBox
    {
      public:
        ImageBoxWithText(Item *parent,
                         const Position &x,
                         const Position &y,
                         const Length &w,
                         const Length &h,
                         Image *image,
                         const UTF8 &description);
        ImageBoxWithText(Item *parent, Image *image, const UTF8 &description);
        ~ImageBoxWithText() override = default;

        void setText(const UTF8 &description);
        void setMinimumSizeToFitImage() override;

      protected:
        TextFixedSize *text;
    };
}; // namespace gui
