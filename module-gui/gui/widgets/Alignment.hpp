#pragma once

#include <cstdint>

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

        Horizontal horizontal = Alignment::Horizontal::None;
        Vertical vertical     = Alignment::Vertical::None;

        Alignment() = default;
        Alignment(Horizontal valH, Vertical valV);
        Alignment(Horizontal valH);
        Alignment(Vertical valV);

        Alignment &operator=(const Alignment &alignment);
        bool operator==(const Alignment &alignment);
        bool operator!=(const Alignment &alignment);
    };

} /* namespace gui */

