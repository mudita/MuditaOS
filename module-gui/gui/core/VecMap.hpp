// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * @file VecMap.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 6 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_GUI_GUI_CORE_VECMAP_HPP_
#define MODULE_GUI_GUI_CORE_VECMAP_HPP_

#include "ImageMap.hpp"

namespace gui
{

    /// Vector map item (*.vpi extension) loaded by `ImageManager::loadVecMap`
    class VecMap : public ImageMap
    {
      protected:
        uint8_t alphaColor = 0x0F;

      public:
        VecMap();
        VecMap(uint16_t w, uint16_t h, uint8_t *data);
        gui::Status load(uint8_t *data, uint32_t size = 0) override;

        uint8_t getAlphaColor()
        {
            return alphaColor;
        };
    };

} /* namespace gui */

#endif /* MODULE_GUI_GUI_CORE_VECMAP_HPP_ */
