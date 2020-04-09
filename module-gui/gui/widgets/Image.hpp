#pragma once

#include <list>

#include "Rect.hpp"
#include "../core/DrawCommand.hpp"
#include "../core/PixMap.hpp"
#include "utf8/UTF8.hpp"

namespace gui
{

    /// image element on screen, it will automatically set it's size to fit space of selected image
    class Image : public Rect
    {
      protected:
        /// internal representation of the image
        ImageMap *imageMap = nullptr;

      public:
        Image();
        /// create new Image element and resize it to size needed by selected Image
        Image(const UTF8 &);
        /// Create new Image element on position x,y w&h will be *ignored*
        Image(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const UTF8 = UTF8{""});
        Image(Item *parent, uint32_t x, uint32_t y, const UTF8 imgName = UTF8{""})
            : Image(parent, x, y, 0u, 0u, imgName)
        {}
        virtual ~Image() = default;
        /// set image with id
        bool set(int id);
        /// set image with string name
        void set(const UTF8 &name);

        // virtual methods from Item
        std::list<DrawCommand *> buildDrawList();
    };

} /* namespace gui */
