// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FontKerning.hpp"
#include "Common.hpp" // for Status, Status::GUI_SUCCESS
#include <cstring>    // for memcpy

typedef uint32_t ucode32;

namespace gui
{
    gui::Status FontKerning::load(uint8_t *data, uint32_t &offset)
    {
        // utf16 id of the first character
        memcpy(&first, data + offset, sizeof(ucode32));
        offset += sizeof(ucode32);
        // utf16 id of the following character
        memcpy(&second, data + offset, sizeof(ucode32));
        offset += sizeof(ucode32);
        // distance in pixels between beginning of first character and beginning of second character
        memcpy(&amount, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);

        return gui::Status::GUI_SUCCESS;
    }
} // namespace gui
