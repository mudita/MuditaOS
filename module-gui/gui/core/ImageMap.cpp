// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ImageMap.hpp"

namespace gui
{

    ImageMap::ImageMap() : id{0}, width{0}, height{0}, data{nullptr}, name{""}
    {}

    ImageMap::ImageMap(uint16_t w, uint16_t h, uint8_t *data) : id{0}, width{w}, height{h}, name{""}
    {}

    ImageMap::~ImageMap()
    {
        if (data)
            delete[] data;
        data = nullptr;
    }
} /* namespace gui */
