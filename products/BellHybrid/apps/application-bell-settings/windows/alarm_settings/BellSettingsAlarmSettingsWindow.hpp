// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-bell-settings/ApplicationBellSettings.hpp>

#include <apps-common/windows/OptionWindow.hpp>

namespace gui
{
    class BellSettingsAlarmSettingsWindow : public OptionWindow
    {
      public:
        static constexpr auto name = "BellSettingsAlarmSettingsWindow";

        explicit BellSettingsAlarmSettingsWindow(app::Application *app);

      private:
        std::list<Option> alarmSettingsOptionsList();

        void buildInterface() override;

        void rebuild() override;
    };
} /* namespace gui */
