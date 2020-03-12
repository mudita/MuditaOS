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

    /**
     * @brief Class that holds pixels and dimensions of the image
     */
    class PixMap : public ImageMap
    {
      public:
        PixMap();
        PixMap(uint16_t w, uint16_t h, uint8_t *data);
        ~PixMap();
        gui::Status load(uint8_t *data, uint32_t size = 0) override;
    };

} /* namespace gui */

#endif /* GUI_CORE_PIXMAP_HPP_ */
