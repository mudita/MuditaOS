// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

        void showImage(bool show);
        void setImage(const UTF8 &name);
        void setMinimumSizeToFitImage();

      private:
        Image *image;
    };
}; // namespace gui
