// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * PixMap.hpp
 *
 *  Created on: 18 maj 2019
 *      Author: robert
 */

#ifndef GUI_CORE_PIXMAP_HPP_
#define GUI_CORE_PIXMAP_HPP_

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
        PixMap(uint16_t w, uint16_t h, uint8_t *data);
        gui::Status load(uint8_t *data, uint32_t size = 0) override;
    };

} /* namespace gui */

#endif /* GUI_CORE_PIXMAP_HPP_ */
