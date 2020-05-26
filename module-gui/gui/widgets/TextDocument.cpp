#include "TextDocument.hpp"
#include <cassert>

namespace gui
{
    TextDocument::TextDocument(const UTF8 &text)
    {
        lines     = splitToTextBlock(text);
        firstLine = lines.begin();
        lastLine  = lines.begin();
    }

    UTF8 TextDocument::getText()
    {
        UTF8 output;
        // iterate over all lines and add content from each line to output string.
        auto it = lines.begin();

        for (auto el : lines) {
            assert(el);
            output += el->text;
            if (el->endType == LineEndType::BREAK) {
                output.insert("\n");
            }
            if (el->endType == LineEndType::CONTINUE_SPACE) {
                output.insert(" ");
            }
            ++it;
        }

        return output;
    }
} // namespace gui
