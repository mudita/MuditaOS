// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <cstring>
#include "VecMap.hpp"

namespace gui
{
    VecMap::VecMap()
    {
        width  = 0;
        height = 0;
        name   = {};
        type   = Type::Vecmap;
    }

    VecMap::VecMap(std::uint16_t w, std::uint16_t h, std::uint8_t *vecMapData) : ImageMap(w, h, vecMapData)
    {
        // Add empty vectors for all rows
        data = new std::uint8_t[height];
        type = Type::Vecmap;

        // No data provided - allocate buffer and clear it with white color
        if (vecMapData == nullptr) {
            std::memset(data, 0x00, height);
        }
    }

    auto VecMap::load(std::uint8_t *vecMapData, std::uint32_t size) -> gui::Status
    {
        std::uint32_t offset = 0;

        // Read width and height of the image
        std::memcpy(&width, &vecMapData[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);
        std::memcpy(&height, &vecMapData[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);
        std::memcpy(&alphaColor, &vecMapData[offset], sizeof(std::uint8_t));
        offset += sizeof(std::uint8_t);

        constexpr auto metadataSize = 2 * sizeof(std::uint16_t) + sizeof(std::uint8_t);

        data = new (std::nothrow) std::uint8_t[size - metadataSize];
        if (data == nullptr) {
            return gui::Status::GUI_FAILURE;
        }

        std::memcpy(data, &vecMapData[offset], size - metadataSize);
        return gui::Status::GUI_SUCCESS;
    }
} /* namespace gui */
