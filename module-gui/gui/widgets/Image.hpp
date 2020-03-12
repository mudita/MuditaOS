/*
 * Image.hpp
 *
 *  Created on: 17 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_IMAGE_HPP_
#define MIDDLEWARES_GUI_WIDGETS_IMAGE_HPP_

#include <list>

#include "Rect.hpp"
#include "../core/DrawCommand.hpp"
#include "../core/PixMap.hpp"
#include "utf8/UTF8.hpp"

namespace gui
{

    class Image : public Rect
    {
      protected:
        // internal representation of the image
        ImageMap *imageMap;

      public:
        Image();
        Image(const UTF8 &);
        Image(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const UTF8 = UTF8{""});
        virtual ~Image();
        bool setImageWithID(int id);

        // virtual methods from Item
        std::list<DrawCommand *> buildDrawList();
    };

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_IMAGE_HPP_ */
