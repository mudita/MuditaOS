#pragma once

#include "Image.hpp"
#include "Label.hpp"

namespace gui
{

    // Icon widget with custom image and label
    class Icon : public Rect
    {
      protected:
        Image *img   = nullptr;
        Label *label = nullptr;

      public:
        Icon() = delete;
        Icon(Item *parent,
             const uint32_t &x,
             const uint32_t &y,
             const std::string &imageName,
             const std::string &labelStr);
        virtual ~Icon() = default;
    };
} // namespace gui