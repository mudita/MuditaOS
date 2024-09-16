// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "NotificationTilesBox.hpp"

#include <widgets/Style.hpp>
#include <Image.hpp>
#include <Style.hpp>

namespace gui
{
    NotificationTilesBox::NotificationTilesBox(Item *parent,
                                               std::shared_ptr<NotificationTilesPresenter> notificationsPresenter)
        : HBox(
              parent, 0, 0, ::style::window::default_body_width, ::style::wallpaper::notificationTiles::tileIconHeight),
          notificationsPresenter(std::move(notificationsPresenter))
    {
        setEdges(RectangleEdge::None);
        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        this->notificationsPresenter->attach(this);
    }
} // namespace gui
