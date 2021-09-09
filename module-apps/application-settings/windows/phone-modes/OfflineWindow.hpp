// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>

#include <Text.hpp>

namespace app::settingsInterface
{
    class OfflineSettings;
} // namespace app::settingsInterface

namespace gui
{
    class OfflineWindow : public BaseSettingsWindow
    {
      public:
        OfflineWindow(app::ApplicationCommon *app, app::settingsInterface::OfflineSettings *offlineSettings);
        void buildInterface() override;

      private:
        app::settingsInterface::OfflineSettings *offlineSettings;
        Text *descriptionText = nullptr;
        auto buildOptionsList() -> std::list<Option> override;
        bool changeFlightMode(bool isFlightMode);
        bool isFlightMode = true;
    };
}; // namespace gui
