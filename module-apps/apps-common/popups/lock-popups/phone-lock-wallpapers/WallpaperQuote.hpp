// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "WallpaperBase.hpp"
#include <apps-common/popups/presenter/QuotesPresenter.hpp>
#include <TextFixedSize.hpp>

namespace gui
{
    namespace style
    {
        namespace textBox
        {
            constexpr auto y = 110;
            constexpr auto h = 340;
        } // namespace textBox

        namespace text
        {
            constexpr auto h             = 225;
            constexpr auto topMarigin    = 20;
            constexpr auto bottomMarigin = 30;
            constexpr auto interline     = 12;

        } // namespace text

    } // namespace style

    class WallpaperQuote : public WallpaperBase
    {
      private:
        std::unique_ptr<QuotesPresenter> quotesPresenter;
        VBox *textBox            = nullptr;
        TextFixedSize *quoteText = nullptr;
        Text *authorText         = nullptr;

      public:
        WallpaperQuote(app::ApplicationCommon *app, Item *parent);
        void build() override;
        void show() override;
        void hide() override;
    };

} /* namespace gui */
