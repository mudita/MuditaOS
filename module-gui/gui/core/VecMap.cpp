// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string.h>
#include "VecMap.hpp"

namespace gui
{

    VecMap::VecMap()
    {
        width  = 0;
        height = 0;
        name   = "";

        type = Type::VECMAP;
    }

    VecMap::VecMap(uint16_t w, uint16_t h, uint8_t *data) : ImageMap(w, h, data)
    {

        // add empty vectors for all rows
        this->data = new uint8_t[height];
        type       = Type::VECMAP;

        // no data provided - allocat buffer and clear it with white color
        if (data == nullptr) {
            if (this->data)
                memset(this->data, 0x00, height);
        }
    }

    gui::Status VecMap::load(uint8_t *data, uint32_t size)
    {

        uint32_t offset = 0;

        // read width and height of the image
        memcpy(&width, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        memcpy(&height, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        memcpy(&alphaColor, data + offset, sizeof(uint8_t));
        offset += sizeof(uint8_t);

        this->data = new uint8_t[size - 2 * sizeof(uint16_t) - sizeof(uint8_t)];
        if (this->data)
            memcpy(this->data, data + offset, size - 2 * sizeof(uint16_t) - sizeof(uint8_t));

        return gui::Status::GUI_SUCCESS;
    }

} /* namespace gui */
