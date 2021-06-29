// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/windows/BaseSettingsWindow.hpp>

namespace gui
{
    class CheckBoxWithLabel;

    class WallpaperWindow : public BaseSettingsWindow
    {
      public:
        WallpaperWindow(app::Application *app);

      private:
        void switchHandler(bool &onOffSwitch);
        auto buildOptionsList() -> std::list<Option> override;

        bool isWallpaperLogoSwitchOn   = false;
        bool isWallpaperClockSwitchOn  = false;
        bool isWallpaperQuotesSwitchOn = false;

        Item *quotes;
    };
} // namespace gui
