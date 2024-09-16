// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "ImageMap.hpp"

namespace gui
{
    /// Vector map item (*.vpi extension) loaded by `ImageManager::loadVecMap`
    class VecMap : public ImageMap
    {
      public:
        VecMap();
        VecMap(std::uint16_t w, std::uint16_t h, std::uint8_t *data);

        auto load(std::uint8_t *data, std::uint32_t size = 0) -> gui::Status override;

        [[nodiscard]] auto getAlphaColor() const -> std::uint8_t
        {
            return alphaColor;
        };

      protected:
        std::uint8_t alphaColor = 0x0F;
    };
} /* namespace gui */
