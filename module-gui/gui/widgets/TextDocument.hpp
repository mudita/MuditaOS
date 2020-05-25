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
    };
}; // namespace gui
