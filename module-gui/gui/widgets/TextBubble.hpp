#pragma once

#include "Text.hpp"

namespace gui
{

    class TextBubble : public Text
    {
      public:
        TextBubble(Item *parent, Position x, Position y, Length w, Length h);
        void setYaps(RectangleYapFlags yaps) override;
    };
} // namespace gui
