// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "WallpaperBase.hpp"
#include <apps-common/popups/presenter/QuotesPresenter.hpp>
#include <TextFixedSize.hpp>
#include <notifications/NotificationTilesBox.hpp>

namespace gui
{
    namespace style
    {
        namespace text
        {
            constexpr inline auto h                   = 240;
            constexpr inline auto maxLineCount        = 6;
            constexpr inline auto topMarigin          = 15;
            constexpr inline auto bottomMarigin       = 15;
            constexpr inline auto interline           = 8;
            constexpr inline auto authorBottomMarigin = 42;

        } // namespace text

    } // namespace style

    class WallpaperQuote : public WallpaperBase
    {
      private:
        std::unique_ptr<QuotesPresenter> quotesPresenter;
        std::shared_ptr<NotificationTilesPresenter> notificationsPresenter;
        VBox *wallpaperBox       = nullptr;
        TextFixedSize *quoteText = nullptr;
        Text *authorText         = nullptr;

      public:
        WallpaperQuote(app::ApplicationCommon *app, Item *parent);
        void show() override;
        void hide() override;
        std::shared_ptr<NotificationsPresenter> getNotificationsPresenter() override;
    };

} /* namespace gui */
