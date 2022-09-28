// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ThreeBox.hpp>
#include <Label.hpp>
#include <Image.hpp>

namespace gui::header
{
    enum class BoxSelection
    {
        Left,
        Center,
        Right
    };

    class Header : public HThreeBox<HBox, HBox, HBox>
    {
      private:
        Label *title = nullptr;

        Item *createTitle(const UTF8 &text);

        void hideOuterBoxes();
        void showOuterBoxes();
        void addToLeftBox(Item *item);
        void addToCenterBox(Item *item);
        void addToRightBox(Item *item);

      public:
        Header(Item *parent, Position x, Position y, Length w, Length h);
        void setTitle(const UTF8 &text);
        [[nodiscard]] UTF8 getTitle();
        void setTitleVisibility(bool visibility);

        void navigationIndicatorAdd(Item *item, BoxSelection position);
        void navigationIndicatorRemove(BoxSelection position);
        [[nodiscard]] bool navigationIndicatorVisible(BoxSelection position);
    };
} // namespace gui::header
