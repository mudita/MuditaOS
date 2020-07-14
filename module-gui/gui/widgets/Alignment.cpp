#include "Alignment.hpp"

namespace gui
{

    Alignment::Alignment(Alignment::Horizontal valH, Alignment::Vertical valV) : horizontal(valH), vertical(valV)
    {}

    Alignment::Alignment(Alignment::Horizontal valH) : horizontal(valH)
    {}

    Alignment::Alignment(Alignment::Vertical valV) : vertical(valV)
    {}

    bool Alignment::operator==(const Alignment &alignment) const
    {
        return !(horizontal != alignment.horizontal || vertical != alignment.vertical);
    }

    bool Alignment::operator!=(const Alignment &alignment) const
    {
        return (horizontal != alignment.horizontal || vertical != alignment.vertical);
    }
} /* namespace gui */
