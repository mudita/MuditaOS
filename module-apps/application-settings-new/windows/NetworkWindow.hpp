// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionWindow.hpp"
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

    class NetworkWindow : public OptionWindow
    {
      private:
        auto netOptList() -> std::list<gui::Option>;
        app::settingsInterface::SimParams *simParams;
        app::settingsInterface::OperatorsSettings *operatorsSettings;

      public:
        NetworkWindow(app::Application *app,
                      app::settingsInterface::SimParams *simParams,
                      app::settingsInterface::OperatorsSettings *operatorsSettings);
    };
} // namespace gui
