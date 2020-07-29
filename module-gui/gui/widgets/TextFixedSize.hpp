#pragma once

#include "Text.hpp"

namespace gui
{

    class TextFixedSize : public Text
    {
      protected:
        unsigned int linesCount   = 10;
        Position underlinePadding = 0;

        void drawLines() override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;

      public:
        TextFixedSize(Item *parent, Position x, Position y, Length w, Length h);

        void setLines(const unsigned int val);
        void setUnderlinePadding(Position val);
    };

} // namespace gui
