// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "data/BellAlarmData.hpp"

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class Label;
    class AlarmSetSpinner;
    class TimeSetFmtSpinner;

    class BellMainWindow : public AppWindow
    {
      public:
        explicit BellMainWindow(app::Application *app);

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        bool updateTime() override;

      private:
        auto handleEnterKey(const InputEvent &inputEvent) -> bool;
        auto handleEditModeEntry() -> void;

        TimeSetFmtSpinner *time          = nullptr;
        gui::Label *temperature          = nullptr;
        AlarmSetSpinner *alarmSetSpinner = nullptr;
        bool alarmEditMode               = false;
        BellAlarm::Status alarmStatus    = BellAlarm::Status::DEACTIVATED;
    };
} // namespace gui
