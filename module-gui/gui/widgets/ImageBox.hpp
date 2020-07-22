#pragma once

#include <BoxLayout.hpp>
#include <Image.hpp>

namespace gui
{
    class ImageBox : public HBox
    {

      public:
        ImageBox(
            Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h, Image *image);

        ~ImageBox() override = default;
    };

}; // namespace gui
