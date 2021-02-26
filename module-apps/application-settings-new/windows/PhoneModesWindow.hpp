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
        inline constexpr auto phone_modes_window = "PhoneModes";
    };

    class PhoneModesWindow : public OptionWindow
    {
      private:
        auto modesOptList() -> std::list<gui::Option>;
        app::settingsInterface::SimParams *simParams;
        void rebuild() override;
        app::settingsInterface::OperatorsSettings *operatorsSettings;

      public:
        PhoneModesWindow(app::Application *app,
                         app::settingsInterface::SimParams *simParams,
                         app::settingsInterface::OperatorsSettings *operatorsSettings);
        void onBeforeShow(ShowMode m, SwitchData *d) override;
    };
} // namespace gui
