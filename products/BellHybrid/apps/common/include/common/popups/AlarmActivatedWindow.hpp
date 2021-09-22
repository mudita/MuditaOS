// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/popups/WindowWithTimer.hpp>
#include <AsyncTask.hpp>

struct AlarmEventRecord;

namespace gui
{
    class Icon;

    class AlarmActivatedWindow : public WindowWithTimer, public app::AsyncCallbackReceiver
    {
      public:
        explicit AlarmActivatedWindow(app::ApplicationCommon *app);

      private:
        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void activateAlarm(AlarmEventRecord &alarmEvent);
        bool onAlarmResponseMessage(sys::ResponseMessage *response, ShowMode mode);
        void showAlarmTime(ShowMode mode, time_t alarmTime);
        void returnToPreviousWindow();

        Icon *icon{};
    };
} /* namespace gui */
