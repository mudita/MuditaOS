#pragma once

#include <Text.hpp>
#include <Image.hpp>

namespace gui
{
    // TODO: possibly a general Icon widget should be created (img + text below)
    class Icon : public Rect
    {
        Text *text = nullptr;
        Image *img = nullptr;

      public:
        Icon(Item *parent,
             const uint32_t &x,
             const uint32_t &y,
             const uint32_t &w,
             const uint32_t &h,
             const UTF8 imageName,
             const UTF8 &text);

        ~Icon() override = default;
    };

}; // namespace gui
