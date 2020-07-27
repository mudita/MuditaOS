#pragma once

#include "Text.hpp"

namespace gui
{

    class TextFixedSize : public Text
    {
      protected:
        unsigned int linesCount = 3;

        void drawLines() override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;

      public:
        TextFixedSize(Item *parent, Position x, Position y, Length w, Length h, unsigned int linesCount = 1);

        void setLines(const unsigned int val);
    };

} // namespace gui
