// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "core/Axes.hpp"
#include "gui/Common.hpp"

namespace gui
{
    enum class MarginInAxis
    {
        First,
        Second
    };

    class Margins
    {
      public:
        Position left, top, right, bottom;

        constexpr Margins() : left{0}, top{0}, right{0}, bottom{0}
        {}

        constexpr Margins(const Position left, const Position top, const Position right, const Position bottom)
            : left{left}, top{top}, right{right}, bottom{bottom}
        {}

        [[nodiscard]] Position getSumInAxis(Axis axis) const;
        [[nodiscard]] Position getMarginInAxis(Axis axis, MarginInAxis pos) const;
        bool operator==(const Margins &margins);
        bool operator!=(const Margins &margins);
    };

    using Padding = Margins;

} /* namespace gui */
