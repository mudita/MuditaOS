// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <cstdint>
#include <Common.hpp>

using ucode32 = std::uint32_t;

namespace gui
{
    class FontKerning
    {
      public:
        gui::Status load(std::uint8_t *data, std::uint32_t &offset);
        // UTF16 id of the first character
        ucode32 first;
        // UTF16 id of the following character
        ucode32 second;
        // Distance in pixels between beginning of first character and beginning of second character
        std::int16_t amount;
    };
} // namespace gui
