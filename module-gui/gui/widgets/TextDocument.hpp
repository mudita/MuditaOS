#pragma once

#include <list>
#include "TextLine.hpp"

namespace gui
{
    class TextDocument
    {
      public:
        // holds list of all lines that  text was divided to.
        std::list<TextLine *> lines;
        // pointer to the first visible line of text
        std::list<TextLine *>::iterator firstLine = lines.end();
        // pointer to the last visible line.
        std::list<TextLine *>::iterator lastLine = lines.end();

        // TODO make private
        std::list<TextLine *>::iterator getTextLineByCursorRow(unsigned int row)
        {
            if (row >= std::distance(firstLine, lines.end())) {
                lines.end();
            }
            return std::next(firstLine, row);
        }

        unsigned int getVisibleRowCount()
        {
            // TODO FIX IT!!!!!!!!!!
            if (*firstLine >= *lastLine) {
                return 0;
            }
            return std::distance(firstLine, lastLine);
        }
    };
}; // namespace gui
