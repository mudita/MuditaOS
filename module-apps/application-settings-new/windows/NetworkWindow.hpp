// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionWindow.hpp"
#include "Application.hpp"

namespace app::settingsInterface
{
    class SimParams;
}

namespace gui
{

    namespace window
    {
        inline constexpr auto network_window = "Network";
    };

    class NetworkWindow : public OptionWindow
    {
      private:
        bool operatorsOn;
        app::settingsInterface::SimParams *simParams;
        auto netOptList() -> std::list<gui::Option>;
        void rebuild() override;

      public:
        NetworkWindow(app::Application *app, app::settingsInterface::SimParams *simParams);
        void onBeforeShow(ShowMode m, SwitchData *d) override;
    };
} // namespace gui
