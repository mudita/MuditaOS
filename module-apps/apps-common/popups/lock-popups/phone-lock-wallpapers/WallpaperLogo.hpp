// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "WallpaperBase.hpp"

#include <BoxLayout.hpp>
#include <notifications/NotificationTilesBox.hpp>

namespace gui
{
    namespace style
    {
        namespace logo
        {
            constexpr inline auto logoTopMarigin    = 53;
            constexpr inline auto logoBottomMarigin = 70;
        } // namespace logo

    } // namespace style
    class WallpaperLogo : public WallpaperBase
    {
      private:
        std::shared_ptr<NotificationTilesPresenter> notificationsPresenter;
        VBox *wallpaperBox = nullptr;

      public:
        WallpaperLogo(gui::Item *parent);
        void show() override;
        void hide() override;
        std::shared_ptr<NotificationsPresenter> getNotificationsPresenter() override;
    };

} /* namespace gui */
