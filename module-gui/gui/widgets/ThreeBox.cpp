// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ThreeBox.hpp"

namespace gui
{
    template <class Parent, class First, class Center, class Last>
    ThreeBox<Parent, First, Center, Last>::ThreeBox(Item *parent, Position x, Position y, Length w, Length h)
        : Parent(parent, x, y, w, h)
    {}

    template <class First, class Center, class Last>
    HThreeBox<First, Center, Last>::HThreeBox(Item *parent, Position x, Position y, Length w, Length h)
        : ThreeBox<HBox, First, Center, Last>(parent, x, y, w, h)
    {}

    template <class First, class Center, class Last>
    VThreeBox<First, Center, Last>::VThreeBox(Item *parent, Position x, Position y, Length w, Length h)
        : ThreeBox<VBox, First, Center, Last>(parent, x, y, w, h)
    {}

    template class HThreeBox<HBox, HBox, HBox>;
    template class VThreeBox<VBox, VBox, VBox>;

} // namespace gui
