// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FontInfo.hpp"
#include "Common.hpp" // for Status, Status::GUI_SUCCESS
#include <cstring>    // for memcpy

namespace gui
{
    gui::Status FontInfo::load(uint8_t *data, uint32_t &offset)
    {

        // read up to 63 chars of the fonts name
        char name[64] = {0};
        memcpy(name, data + offset, 63);
        offset += 64;
        face = std::string(name);

        // size of the true type font
        memcpy(&size, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // flag that informs if font is bold
        memcpy(&bold, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // flag that informs if font is italic
        memcpy(&italic, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // flag that informs if smoothing was turned on. 1 - smoothing was turned on.
        memcpy(&smooth, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // TODO additional space between characters????
        memcpy(&char_spacing, data + offset, sizeof(int16_t));
        offset += sizeof(int16_t);
        // TODO additional space between lines
        memcpy(&line_spacing, data + offset, sizeof(int16_t));
        offset += sizeof(int16_t);
        // distance in pixels between each line of text
        memcpy(&line_height, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // number of pixels from the absolute top of the line to the base of the characters
        memcpy(&base, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // width of the texture, normally used to scale the x pos of the character image
        memcpy(&scale_w, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // height of the texture, normally used to scale the y pos of the character image
        memcpy(&scale_h, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);

        return gui::Status::GUI_SUCCESS;
    }
} // namespace gui
