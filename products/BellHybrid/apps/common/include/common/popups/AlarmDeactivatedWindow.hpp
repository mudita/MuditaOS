// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/popups/WindowWithTimer.hpp>
#include <AsyncTask.hpp>

struct AlarmEventRecord;

namespace gui
{
    class Icon;
    class AlarmDeactivatedWindow : public WindowWithTimer, public app::AsyncCallbackReceiver
    {
      public:
        explicit AlarmDeactivatedWindow(app::ApplicationCommon *app);

      private:
        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;
        void returnToPreviousWindow();
        void deactivateAlarm(AlarmEventRecord &alarmEvent);
        bool onAlarmResponseMessage(sys::ResponseMessage *response);

        Icon *icon{};
    };
} /* namespace gui */
