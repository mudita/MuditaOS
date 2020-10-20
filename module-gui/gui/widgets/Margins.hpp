// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "core/Axes.hpp"

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
        short left, top, right, bottom;

        Margins();
        Margins(const short left, const short top, const short right, const short bottom);
        virtual ~Margins();

        [[nodiscard]] short getSumInAxis(Axis axis) const;
        [[nodiscard]] short getMarginInAxis(Axis axis, MarginInAxis pos) const;
        bool operator==(const Margins &margins);
        bool operator!=(const Margins &margins);
    };

    using Padding = Margins;

} /* namespace gui */

