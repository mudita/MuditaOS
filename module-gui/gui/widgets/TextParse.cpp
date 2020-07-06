#include "TextParse.hpp"
#include <sstream>

namespace gui
{
    auto textToTextBlocks(const UTF8 &text, Font *font, TextBlock::End end) -> std::list<TextBlock>
    {
        std::list<TextBlock> lines;
        std::stringstream ss(text.c_str());
        std::string line;
        while (std::getline(ss, line)) {
            lines.emplace_back(TextBlock(line, font, end));
        }
        return lines;
    }
} // namespace gui
