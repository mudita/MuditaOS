// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "TextBlock.hpp"
#include <list>

namespace gui
{
    /// @note with nullptr font - nullptr will be assigned as font
    auto textToTextBlocks(const UTF8 &text, const RawFont *font, TextBlock::End end = TextBlock::End::Newline)
        -> std::list<TextBlock>;
    auto textToTextBlocks(const UTF8 &text, TextFormat format) -> std::list<TextBlock>;
} // namespace gui
