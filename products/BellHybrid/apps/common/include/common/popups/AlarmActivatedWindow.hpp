// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/windows/Dialog.hpp>
#include <AsyncTask.hpp>

struct AlarmEventRecord;

namespace gui
{
    class AlarmActivatedWindow : public Dialog, public app::AsyncCallbackReceiver
    {
      public:
        explicit AlarmActivatedWindow(app::ApplicationCommon *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        void activateAlarm(AlarmEventRecord alarmEvent);
        bool onAlarmResponseMessage(sys::ResponseMessage *response, ShowMode mode);
        void showAlarmTime(ShowMode mode, time_t alarmTime);
    };
} /* namespace gui */
