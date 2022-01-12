// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WallpaperLogo.hpp"
#include <service-appmgr/Controller.hpp>
#include <Style.hpp>
#include <ImageBox.hpp>
#include <Image.hpp>

namespace gui
{
    WallpaperLogo::WallpaperLogo(Item *parent)
        : WallpaperBase(parent), notificationsPresenter(std::make_shared<NotificationTilesPresenter>())
    {
        wallpaperBox = new VBox(parent,
                                ::style::window::default_left_margin,
                                ::style::wallpaper::wallpaperBox::y,
                                ::style::window::default_body_width,
                                ::style::wallpaper::wallpaperBox::h);
        wallpaperBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        wallpaperBox->setEdges(RectangleEdge::None);

        auto logoImage = new ImageBox(wallpaperBox, new Image("logo", ImageTypeSpecifier::W_G));
        logoImage->setMinimumSizeToFitImage();
        logoImage->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        logoImage->setMargins(Margins(0, style::logo::logoTopMarigin, 0, style::logo::logoBottomMarigin));

        new NotificationTilesBox(wallpaperBox, notificationsPresenter);
        hide();
    }

    void WallpaperLogo::show()
    {
        wallpaperBox->setVisible(true);
        wallpaperBox->resizeItems();
    }

    void WallpaperLogo::hide()
    {
        wallpaperBox->setVisible(false);
        wallpaperBox->resizeItems();
    }

    std::shared_ptr<NotificationsPresenter> WallpaperLogo::getNotificationsPresenter()
    {
        return notificationsPresenter;
    }

} /* namespace gui */
