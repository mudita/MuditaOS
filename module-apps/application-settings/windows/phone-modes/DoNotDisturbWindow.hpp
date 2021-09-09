// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>

#include <Text.hpp>

namespace app::settingsInterface
{
    class DndSettings;
} // namespace app::settingsInterface

namespace gui
{
    class DoNotDisturbWindow : public BaseSettingsWindow
    {
      public:
        DoNotDisturbWindow(app::ApplicationCommon *app, app::settingsInterface::DndSettings *dndSettings);
        void buildInterface() override;

      private:
        app::settingsInterface::DndSettings *dndSettings = nullptr;
        Text *descriptionText                            = nullptr;
        auto buildOptionsList() -> std::list<Option> override;
    };
} // namespace gui
