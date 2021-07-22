// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <data/BellAlarmData.hpp>
#include <AppWindow.hpp>
#include <gui/widgets/Spinner.hpp>
#include <widgets/TimeSetSpinner.hpp>
#include <widgets/AlarmSetSpinner.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    class BellMainWindow : public AppWindow
    {
      public:
        explicit BellMainWindow(app::Application *app);

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        bool updateTime() override;

        gui::Label *time        = nullptr;
        gui::Label *temperature = nullptr;
        AlarmSetSpinner *alarmSetSpinner = nullptr;

      private:
        auto handleEnterKey(const InputEvent &inputEvent) -> bool;
        auto handleEditModeEntry() -> void;
        bool alarmEditMode            = false;
        BellAlarm::Status alarmStatus = BellAlarm::Status::DEACTIVATED;
    };
} // namespace gui
