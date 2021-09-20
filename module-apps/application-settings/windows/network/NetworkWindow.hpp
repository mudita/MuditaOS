// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace app::settingsInterface
{
    class OperatorsSettings;
}; // namespace app::settingsInterface

namespace gui
{
    class NetworkWindow : public BaseSettingsWindow
    {
      private:
        auto buildOptionsList() -> std::list<Option> override;
        [[maybe_unused]] app::settingsInterface::OperatorsSettings *operatorsSettings;

      public:
        NetworkWindow(app::ApplicationCommon *app, app::settingsInterface::OperatorsSettings *operatorsSettings);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };
} // namespace gui
