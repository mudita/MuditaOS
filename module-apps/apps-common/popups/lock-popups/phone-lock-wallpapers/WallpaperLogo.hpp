// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "WallpaperBase.hpp"

#include <ImageBox.hpp>

namespace gui
{
    namespace style
    {
        namespace logo
        {
            constexpr auto y = 163;
        } // namespace logo

    } // namespace style
    class WallpaperLogo : public WallpaperBase
    {
      private:
        ImageBox *logoImage = nullptr;

      public:
        WallpaperLogo(gui::Item *parent);
        void build() override;
        void show() override;
        void hide() override;
    };

} /* namespace gui */
