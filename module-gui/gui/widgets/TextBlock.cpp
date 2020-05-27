#include "TextBlock.hpp"

namespace gui
{
    UTF8 TextBlock::getText()
    {
        return text;
    }

    UTF8 TextBlock::getTextWithEnding()
    {
        UTF8 retText = text;
        if (endType == LineEndType::BREAK) {
            retText.insert("\n");
        }
        else if (endType == LineEndType::CONTINUE_SPACE) {
            retText.insert(" ");
        }
        return retText;
    }
} // namespace gui
