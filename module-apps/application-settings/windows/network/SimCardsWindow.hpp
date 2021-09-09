// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace app::settingsInterface
{
    class SimParams;
}; // namespace app::settingsInterface

namespace gui
{
    class SimCardsWindow : public BaseSettingsWindow
    {
      private:
        auto buildOptionsList() -> std::list<Option> override;
        app::settingsInterface::SimParams *simParams;

      public:
        SimCardsWindow(app::ApplicationCommon *app, app::settingsInterface::SimParams *simParams);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };
} // namespace gui
