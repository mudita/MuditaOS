// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <core/TextBlock.hpp>
#include <list>

namespace gui
{
    /// @note with nullptr font - nullptr will be assigned as font
    auto textToTextBlocks(const UTF8 &text, const RawFont *font, TextBlock::End end = TextBlock::End::Newline)
        -> std::list<TextBlock>;
    auto textToTextBlocks(const UTF8 &text, TextFormat format) -> std::list<TextBlock>;

    auto addEmptyBlockOnNewline(std::list<TextBlock> &blocks, const RawFont *font) -> void;
    auto addEmptyBlockOnNewline(std::list<TextBlock> &blocks, TextFormat format) -> void;
} // namespace gui
