// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <module-gui/gui/Common.hpp>

namespace gui
{
    class Alignment
    {
      public:
        enum class Vertical
        {
            Top,
            Center,
            Bottom,
            None,
        };

        enum class Horizontal
        {
            Right,
            Center,
            Left,
            None,
        };

        Horizontal horizontal = Alignment::Horizontal::Left;
        Vertical vertical     = Alignment::Vertical::Top;

        constexpr Alignment() = default;

        constexpr Alignment(Horizontal valH, Vertical valV) : horizontal(valH), vertical(valV)
        {}

        constexpr Alignment(Horizontal valH) : horizontal(valH)
        {}

        constexpr Alignment(Vertical valV) : vertical(valV)
        {}

        [[nodiscard]] Position calculateHAlignment(Length parentSize, Length childSize) const;
        [[nodiscard]] Position calculateVAlignment(Length parentSize, Length childSize) const;

        bool operator==(const Alignment &alignment) const;
        bool operator!=(const Alignment &alignment) const;
    };

} /* namespace gui */
