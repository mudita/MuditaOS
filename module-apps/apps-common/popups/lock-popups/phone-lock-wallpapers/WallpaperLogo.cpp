// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WallpaperLogo.hpp"
#include <service-appmgr/Controller.hpp>
#include <Style.hpp>
#include <Image.hpp>

namespace gui
{
    WallpaperLogo::WallpaperLogo(Item *parent) : WallpaperBase(parent)
    {}

    void WallpaperLogo::build()
    {
        logoImage = new ImageBox(parent,
                                 0,
                                 style::logo::y,
                                 ::style::window_width,
                                 ::style::window::default_body_height - style::logo::y,
                                 new Image("logo", ImageTypeSpecifier::W_G));
        logoImage->setMinimumSizeToFitImage();
        logoImage->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        hide();
    }

    void WallpaperLogo::show()
    {
        logoImage->setVisible(true);
        logoImage->resizeItems();
    }

    void WallpaperLogo::hide()
    {
        logoImage->setVisible(false);
        logoImage->resizeItems();
    }

} /* namespace gui */
