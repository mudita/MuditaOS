// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ImageMap.hpp"

namespace gui
{
    ImageMap::ImageMap() : id{0}, width{0}, height{0}, data{nullptr}
    {}

    ImageMap::ImageMap(uint16_t w, uint16_t h, uint8_t *data) : id{0}, width{w}, height{h}
    {}

    ImageMap::~ImageMap()
    {
        delete[] data;
        data = nullptr;
    }
} /* namespace gui */
