#include "TextParse.hpp"
#include <sstream>

namespace gui
{
    auto textToTextBlocks(const UTF8 &text, const RawFont *font, TextBlock::End end) -> std::list<TextBlock>
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
            if (!blocks.empty()) {
                blocks.back().setEnd(TextBlock::End::Newline);
            }
            blocks.emplace_back(TextBlock(line, std::make_unique<TextFormat>(format)));
        }
        return blocks;
    }
} // namespace gui
