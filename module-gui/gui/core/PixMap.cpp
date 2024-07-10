// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PixMap.hpp"
#include <cstring>

namespace gui
{
    PixMap::PixMap()
    {
        width  = 0;
        height = 0;
        name   = {};
        type   = Type::Pixmap;
    }

    PixMap::PixMap(std::uint16_t w, std::uint16_t h, std::uint8_t *pixMapData) : ImageMap(w, h, pixMapData)
    {
        data = new std::uint8_t[width * height];
        type = Type::Pixmap;

        // No data provided - allocate buffer and clear it with white color
        if (pixMapData == nullptr) {
            std::memset(data, 0x0F, width * height);
        }
        else {
            std::memcpy(data, pixMapData, width * height);
        }
    }

    gui::Status PixMap::load(std::uint8_t *pixMapData, std::uint32_t size)
    {
        std::uint32_t offset = 0;

        // Read width and height of the image
        std::memcpy(&width, &pixMapData[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);
        std::memcpy(&height, &pixMapData[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);

        data = new (std::nothrow) std::uint8_t[width * height];
        if (data == nullptr) {
            return Status::GUI_FAILURE;
        }
        std::memcpy(data, &pixMapData[offset], width * height);
        return gui::Status::GUI_SUCCESS;
    }
} /* namespace gui */
