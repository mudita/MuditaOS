#include "Alignment.hpp"

namespace gui
{

    Alignment::Alignment(Alignment::Horizontal valH, Alignment::Vertical valV) : horizontal(valH), vertical(valV)
    {}

    Alignment::Alignment(Alignment::Horizontal valH) : horizontal(valH)
    {}

    Alignment::Alignment(Alignment::Vertical valV) : vertical(valV)
    {}

    Alignment &Alignment::operator=(const Alignment &alignment)
    {
        if (alignment.horizontal != Alignment::Horizontal::None)
            this->horizontal = alignment.horizontal;

        if (alignment.vertical != Alignment::Vertical::None)
            this->vertical = alignment.vertical;

        return *this;
    }

    bool Alignment::operator==(const Alignment &alignment)
    {
        return !(horizontal != alignment.horizontal || vertical != alignment.vertical);
    }

    bool Alignment::operator!=(const Alignment &alignment)
    {
        return (horizontal != alignment.horizontal || vertical != alignment.vertical);
    }
} /* namespace gui */
