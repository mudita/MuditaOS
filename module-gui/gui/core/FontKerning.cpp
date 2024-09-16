// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "FontKerning.hpp"
#include "Common.hpp"
#include <cstring>

namespace gui
{
    gui::Status FontKerning::load(uint8_t *data, uint32_t &offset)
    {
        // UTF16 id of the first character
        std::memcpy(&first, &data[offset], sizeof(ucode32));
        offset += sizeof(ucode32);

        // UTF16 id of the following character
        std::memcpy(&second, &data[offset], sizeof(ucode32));
        offset += sizeof(ucode32);

        // Distance in pixels between beginning of first character and beginning of second character
        std::memcpy(&amount, &data[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);

        return gui::Status::GUI_SUCCESS;
    }
} // namespace gui
