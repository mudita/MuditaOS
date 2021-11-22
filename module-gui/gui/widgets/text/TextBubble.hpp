// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Text.hpp"

namespace gui
{

    class TextBubble : public Text
    {
      public:
        TextBubble(Item *parent, Position x, Position y, Length w, Length h);
        void setYaps(RectangleYap yaps) override;
    };
} // namespace gui
