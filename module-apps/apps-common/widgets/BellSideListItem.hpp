// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "BellBaseLayout.hpp"
#include "BellSideListItemStyle.hpp"
#include <ListItem.hpp>

namespace gui
{
    class TextFixedSize;

    class BellSideListItem : public ListItem
    {
      public:
        void setBottomDescriptionText(const std::string &description);

      protected:
        explicit BellSideListItem(BellBaseLayout::LayoutType type = BellBaseLayout::LayoutType::WithArrows);
        void setupBottomTextBox(const std::string &description);
        void setupTopTextBox(const std::string &description);

        BellBaseLayout *body      = nullptr;
        TextFixedSize *bottomText = nullptr;

      private:
        void setupTextBox(TextFixedSize *textBox, const std::string &fontName, const std::string &description);
    };
} /* namespace gui */
