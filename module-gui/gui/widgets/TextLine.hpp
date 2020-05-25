#pragma once

#include "TextConstants.hpp"
#include "utf8/UTF8.hpp"

namespace gui
{
    class TextLine
    {
      public:
        UTF8 text;
        uint32_t startIndex  = 0;
        uint32_t endIndex    = 0;
        LineEndType endType  = LineEndType::EOT;
        uint32_t pixelLength = 0;

        TextLine(const UTF8 &text, uint32_t startIndex, uint32_t endIndex, LineEndType endType, uint32_t pixelLength);
        /**
         * Returns text with appropriate ending.
         */
        UTF8 getText()
        {
            return text;
        }
        UTF8 getTextWithEnding()
        {
            UTF8 retText = text;
            if (endType == LineEndType::BREAK)
                retText.insert("\n");
            else if (endType == LineEndType::CONTINUE_SPACE)
                retText.insert(" ");
            return retText;
        }
    };
} // namespace gui
