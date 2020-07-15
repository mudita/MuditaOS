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

        Horizontal horizontal = Alignment::Horizontal::Left;
        Vertical vertical     = Alignment::Vertical::Top;

        Alignment()                  = default;
        Alignment(const Alignment &) = default;
        Alignment(Horizontal valH, Vertical valV);
        Alignment(Horizontal valH);
        Alignment(Vertical valV);

        bool operator==(const Alignment &alignment) const;
        bool operator!=(const Alignment &alignment) const;
    };

} /* namespace gui */
