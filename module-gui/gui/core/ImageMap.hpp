// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef MODULE_GUI_GUI_CORE_IMAGEMAP_HPP_
#define MODULE_GUI_GUI_CORE_IMAGEMAP_HPP_

#include <cstdint>
#include <string>
#include "../Common.hpp"

namespace gui
{

    /*
     *
     */
    class ImageMap
    {
      public:
        enum class Type
        {
            NONE,
            PIXMAP,
            VECMAP
        };

      protected:
        // id of the pixmap asigned by the pixmap manager
        uint32_t id;
        // number of columns in the pixmap
        uint16_t width;
        // number of rows in the image
        uint16_t height;
        // data of the image
        uint8_t *data = nullptr;
        // file name
        std::string name;
        // type of the image
        Type type = Type::NONE;

      public:
        ImageMap();
        ImageMap(uint16_t w, uint16_t h, uint8_t *data);
        virtual ~ImageMap();

        Type getType()
        {
            return type;
        };
        uint16_t getWidth()
        {
            return width;
        };
        uint16_t getHeight()
        {
            return height;
        };
        uint8_t *getData()
        {
            return data;
        };
        std::string getName()
        {
            return name;
        };
        uint32_t getID()
        {
            return id;
        };

        void setID(uint32_t id)
        {
            this->id = id;
        };
        void setName(std::string name)
        {
            this->name = name;
        };

        virtual gui::Status load(uint8_t *data, uint32_t size = 0)
        {
            return gui::Status::GUI_SUCCESS;
        };
    };

} /* namespace gui */

#endif /* MODULE_GUI_GUI_CORE_IMAGEMAP_HPP_ */
