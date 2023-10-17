// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>
#include <Text.hpp>
#include <Image.hpp>

namespace gui
{
    namespace icon
    {
        inline constexpr auto image_top_margin    = 30;
        inline constexpr auto image_bottom_margin = 33;
    } // namespace icon

    class Icon : public VBox
    {
      public:
        Icon(Item *parent,
             const uint32_t &x,
             const uint32_t &y,
             const uint32_t &w,
             const uint32_t &h,
             const UTF8 &imageName,
             const UTF8 &text,
             ImageTypeSpecifier specifier = ImageTypeSpecifier::None);

        ~Icon() override = default;

        Text *text   = nullptr;
        Image *image = nullptr;
    };

}; // namespace gui
