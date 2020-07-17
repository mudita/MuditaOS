#pragma once

#include "Text.hpp"

namespace gui
{

    class TextFixedSize : public Text
    {
      protected:
        unsigned int linesCount = 1;

        void drawLines() override;

      public:
        TextFixedSize(Item *parent, Position x, Position y, Length w, Length h, unsigned int linesCount = 1);

        void setLines(const unsigned int val);
    };

} // namespace gui
