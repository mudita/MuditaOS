#pragma once

#include <list>
#include "TextBlock.hpp"

namespace gui
{
    class TextDocument
    {
      public:
        TextDocument(const UTF8 &text = "");

        std::list<TextBlock *> splitToTextBlock(const UTF8 &text) // text
        {
            std::list<TextBlock *> lines;
            lines.push_back(new TextBlock(text, 0, 0, LineEndType::EOT, 0));
            return lines;
        }

        void append(TextBlock *&&text)
        {
            lines.push_back(text);
        }

        UTF8 getText();

        // holds list of all lines that  text was divided to.
        std::list<TextBlock *> lines;
        // pointer to the first visible line of text
        std::list<TextBlock *>::iterator firstLine = lines.end();
        // pointer to the last visible line.
        std::list<TextBlock *>::iterator lastLine = lines.end();

        std::list<TextBlock *>::iterator getTextBlockByCursorRow(unsigned int row)
        {
            if (row >= std::distance(firstLine, lines.end())) {
                lines.end();
            }
            return std::next(firstLine, row);
        }

        unsigned int getVisibleRowCount()
        {
            if (*firstLine >= *lastLine) {
                return 0;
            }
            return std::distance(firstLine, lastLine);
        }

        void destroy()
        {
            for (auto el : lines) {
                delete el;
            }
            lines.clear();
        }

        ~TextDocument()
        {
            destroy();
        }

    };
}; // namespace gui
