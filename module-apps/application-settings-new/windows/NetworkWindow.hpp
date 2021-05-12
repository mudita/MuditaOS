// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"
#include "Application.hpp"

namespace app::settingsInterface
{
    class SimParams;
    class OperatorsSettings;
}; // namespace app::settingsInterface

namespace gui
{

    namespace window
    {
        inline constexpr auto network_window = "Network";
    };

    class NetworkWindow : public BaseSettingsWindow
    {
      private:
        auto buildOptionsList() -> std::list<Option> override;
        app::settingsInterface::SimParams *simParams;
        app::settingsInterface::OperatorsSettings *operatorsSettings;

      public:
        NetworkWindow(app::Application *app,
                      app::settingsInterface::SimParams *simParams,
                      app::settingsInterface::OperatorsSettings *operatorsSettings);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };
} // namespace gui
