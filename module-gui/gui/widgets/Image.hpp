// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
      public:
        Image();
        /// create new Image element and resize it to size needed by selected Image
        explicit Image(const UTF8 &imageName, ImageTypeSpecifier specifier = ImageTypeSpecifier::None);
        Image(Item *parent, const UTF8 &imageName, ImageTypeSpecifier specifier = ImageTypeSpecifier::None);
        /// Create new Image element on position x,y w&h will be *ignored*
        Image(Item *parent,
              std::uint32_t x,
              std::uint32_t y,
              std::uint32_t w,
              std::uint32_t h,
              const UTF8 &imageName = UTF8{""});
        Image(Item *parent, std::uint32_t x, std::uint32_t y, const UTF8 &imageName = UTF8{""})
            : Image(parent, x, y, 0U, 0U, imageName)
        {}

        bool set(std::uint32_t id);
        void set(const UTF8 &name, ImageTypeSpecifier specifier = ImageTypeSpecifier::None);

        void buildDrawListImplementation(std::list<Command> &commands) override;
        void accept(GuiVisitor &visitor) override;

      protected:
        /// internal representation of the image
        ImageMap *imageMap = nullptr;
    };
} /* namespace gui */
