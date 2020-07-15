#include "TextParse.hpp"
#include <sstream>

namespace gui
{
    auto textToTextBlocks(const UTF8 &text, RawFont *font, TextBlock::End end) -> std::list<TextBlock>
    {
        std::list<TextBlock> blocks;
        std::stringstream ss(text.c_str());
        std::string line;
        while (std::getline(ss, line)) {
            blocks.emplace_back(TextBlock(line, font, end));
        }
        return blocks;
    }

    auto textToTextBlocks(const UTF8 &text, TextFormat format) -> std::list<TextBlock>
    {
        std::list<TextBlock> blocks;
        std::stringstream ss(text.c_str());
        std::string line;
        while (std::getline(ss, line)) {
            blocks.emplace_back(TextBlock(line, std::make_unique<TextFormat>(format)));
            blocks.back().setEnd(TextBlock::End::Newline);
        }
        return blocks;
    }
} // namespace gui
