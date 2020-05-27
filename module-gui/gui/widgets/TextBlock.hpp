#pragma once

#include "TextConstants.hpp"
#include "utf8/UTF8.hpp"

namespace gui
{
    class TextBlock
    {
      public:
        UTF8 text;
        uint32_t startIndex  = 0;
        uint32_t endIndex    = 0;
        LineEndType endType  = LineEndType::EOT;
        uint32_t pixelLength = 0;

        TextBlock(const UTF8 &text, uint32_t startIndex, uint32_t endIndex, LineEndType endType, uint32_t pixelLength);
        UTF8 getText();
        UTF8 getTextWithEnding();
    };
} // namespace gui
