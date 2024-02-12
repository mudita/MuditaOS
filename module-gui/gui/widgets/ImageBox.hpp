// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>
#include <Image.hpp>

namespace gui
{
    class ImageBox : public VBox
    {
      public:
        ImageBox(Item *parent, const Position &x, const Position &y, const Length &w, const Length &h, Image *image);
        ImageBox(Item *parent, Image *image);

        ~ImageBox() override = default;

        void showImage(bool show);
        void setImage(const UTF8 &name, ImageTypeSpecifier specifier = ImageTypeSpecifier::None);
        /// adjusts the box size perfectly to the image
        void fitBoxToImage();
        /// adjusts the box size to at least the same size as the image
        virtual void setMinimumSizeToFitImage();

      protected:
        Image *image;
    };
}; // namespace gui
