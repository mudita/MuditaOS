// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <Style.hpp>
#include <string>

namespace style
{
    namespace settings
    {
        namespace window
        {
            namespace leftArrowImage
            {
                constexpr uint32_t x = style::window::default_left_margin;
                constexpr uint32_t y = 62;
                constexpr uint32_t w = 11;
                constexpr uint32_t h = 13;
            } // namespace leftArrowImage

            namespace crossImage
            {
                constexpr uint32_t x = style::window::default_left_margin + 20;
                constexpr uint32_t y = 55;
                constexpr uint32_t w = 24;
                constexpr uint32_t h = 24;
            } // namespace crossImage
        }     // namespace window
    };        // namespace settings
} // namespace style
