#pragma once

#include "Text.hpp"
#include "Image.hpp"
#include "BoxLayout.hpp"

namespace gui
{

    class EmptyListWidget : public Rect
    {
        Text *text  = nullptr;
        Image *icon = nullptr;

      public:
        EmptyListWidget(Item *parent,
                        const uint32_t &x,
                        const uint32_t &y,
                        const uint32_t &w,
                        const uint32_t &h,
                        const UTF8 imageName,
                        const UTF8 &text);
        // EmptyListWidget(Item *parent, const uint32_t &x, const uint32_t &y, const uint32_t &w, const uint32_t &h,
        // BoundingBox iconCords, BoundingBox textCords); EmptyListWidget(Item *parent, const uint32_t &x, BoundingBox
        // cords, BoundingBox iconCords, BoundingBox textCords);
        ~EmptyListWidget() override = default;

        // virtual void setVisible(bool value) override;
    };

}; // namespace gui
