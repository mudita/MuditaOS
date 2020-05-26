#pragma once

#include <list>
#include "TextLine.hpp"

namespace gui
{
    class TextDocument
    {
      public:
        TextDocument(const UTF8 &text = "");

        // TODO use TextChunk and TextLines instead, make some "logic"
        std::list<TextLine *> splitToTextChunk(const UTF8 &text) // text
        {
            std::list<TextLine *> lines;
            lines.push_back(new TextLine(text, 0, 0, LineEndType::EOT, 0));
            return lines;
        }

        void append(TextLine *&&text)
        {
            lines.push_back(text);
        }

        UTF8 getText();

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

        // Instead working on TextLine -> work on TextBlock ( Text & it's params (underline, bold, size etc) )
        //        // TODO -> add whole chunk
        //        void addTextBlock();
        //
        //        // TODO -> append to current chunk
        //        void addTextChar();
    };
}; // namespace gui
