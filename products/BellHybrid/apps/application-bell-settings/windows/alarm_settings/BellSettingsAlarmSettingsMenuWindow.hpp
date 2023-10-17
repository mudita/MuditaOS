// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-bell-settings/ApplicationBellSettings.hpp>

#include <common/options/BellOptionWindow.hpp>

namespace gui
{
    class BellSettingsAlarmSettingsMenuWindow : public BellOptionWindow
    {
      public:
        static constexpr auto name = "BellSettingsAlarmSettingsMenuWindow";

        explicit BellSettingsAlarmSettingsMenuWindow(app::ApplicationCommon *app);

      private:
        std::list<Option> alarmSettingsOptionsList();
    };
} /* namespace gui */
