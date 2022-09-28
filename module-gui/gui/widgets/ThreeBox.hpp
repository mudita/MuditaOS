// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>

namespace gui
{
    template <class Parent, class First, class Center, class Last>
    class ThreeBox : public Parent
    {
      public:
        explicit ThreeBox(Item *parent, Position x = 0, Position y = 0, Length w = 0, Length = 0);
        virtual ~ThreeBox() = default;

        First *firstBox   = nullptr;
        Center *centerBox = nullptr;
        Last *lastBox     = nullptr;
    };

    template <class First, class Center, class Last>
    class HThreeBox : public ThreeBox<HBox, First, Center, Last>
    {
      public:
        explicit HThreeBox(Item *parent, Position x = 0, Position y = 0, Length w = 0, Length h = 0);
    };

    template <class First, class Center, class Last>
    class VThreeBox : public ThreeBox<VBox, First, Center, Last>
    {
      public:
        explicit VThreeBox(Item *parent, Position x = 0, Position y = 0, Length w = 0, Length = 0);
    };
}; // namespace gui
