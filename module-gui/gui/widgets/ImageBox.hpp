// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
        void setImage(const UTF8 &name);
        virtual void setMinimumSizeToFitImage();

      protected:
        Image *image;
    };
}; // namespace gui
