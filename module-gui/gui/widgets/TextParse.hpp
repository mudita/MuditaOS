#pragma once

#include "TextBlock.hpp"
#include <list>

namespace gui
{
    /// @note with nullptr font - nullptr will be assigned as font
    auto textToTextBlocks(const UTF8 &text, Font *font, TextBlock::End end = TextBlock::End::Newline)
        -> std::list<TextBlock>;
} // namespace gui
