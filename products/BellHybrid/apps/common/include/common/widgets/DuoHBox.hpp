// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BoxLayout.hpp"

namespace gui
{

    class DuoHBox : public HBox
    {
      public:
        DuoHBox(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h) : HBox(parent, x, y, w, h){};
        void setItems(Item *leftItem, Item *rightItem);
        void resizeItems() override;
        auto handleRequestResize(const Item *, Length request_w, Length request_h) -> Size override;

      private:
        void setAlignements();

        Item *leftItem{};
        Item *rightItem{};
    };
} // namespace gui
