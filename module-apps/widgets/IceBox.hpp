// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>
#include <Label.hpp>
#include <Image.hpp>

namespace style::window::ice
{
    constexpr inline auto x = 25;
    constexpr inline auto y = 53;
    constexpr inline auto w = 60;
    constexpr inline auto h = 38;

    constexpr inline auto margin = 3;

    namespace text
    {
        constexpr inline auto w = 40;
    }
} // namespace style::window::ice

namespace gui
{
    class IceBox : public HBox
    {
      public:
        explicit IceBox(Item *parent = nullptr);
    };
} // namespace gui
