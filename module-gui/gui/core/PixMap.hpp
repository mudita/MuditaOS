// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <cstring>
#include <cstdint>

#include "ImageMap.hpp"

#include "../Common.hpp"

namespace gui
{
    /// Pixel map item (*.mpi extension) loaded by `ImageManager::loadVecMap`
    class PixMap : public ImageMap
    {
      public:
        PixMap();
        PixMap(std::uint16_t w, std::uint16_t h, std::uint8_t *pixMapData);

        auto load(std::uint8_t *pixMapData, std::uint32_t size = 0) -> gui::Status override;
    };
} /* namespace gui */
