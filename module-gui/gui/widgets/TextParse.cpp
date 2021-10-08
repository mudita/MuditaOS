// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

        addEmptyBlockOnNewline(blocks, font);
        return blocks;
    }

    auto textToTextBlocks(const UTF8 &text, TextFormat format) -> std::list<TextBlock>
    {
        std::list<TextBlock> blocks;
        std::stringstream ss(text.c_str());
        std::string line;
        while (std::getline(ss, line)) {
            auto block = TextBlock(line, std::make_unique<TextFormat>(format));
            if (ss.good()) {
                block.setEnd(TextBlock::End::Newline);
            }
            blocks.emplace_back(block);
        }

        addEmptyBlockOnNewline(blocks, format);
        return blocks;
    }

    auto addEmptyBlockOnNewline(std::list<TextBlock> &blocks, const RawFont *font) -> void
    {
        if (!blocks.empty() && blocks.back().getEnd() == TextBlock::End::Newline) {
            blocks.emplace_back(TextBlock("", font, TextBlock::End::None));
        }
    }

    auto addEmptyBlockOnNewline(std::list<TextBlock> &blocks, TextFormat format) -> void
    {
        if (!blocks.empty() && blocks.back().getEnd() == TextBlock::End::Newline) {
            blocks.emplace_back(TextBlock("", std::make_unique<TextFormat>(format)));
            blocks.back().setEnd(TextBlock::End::None);
        }
    }
} // namespace gui
