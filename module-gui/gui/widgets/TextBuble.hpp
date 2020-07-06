#pragma once

#include "Text.hpp"

namespace gui
{

    class TextBuble : public Text
    {
      public:
        TextBuble(Item *parent, Position x, Position y, Length w, Length h);
        void setYaps(RectangleYapFlags yaps) override;
    };
} // namespace gui
