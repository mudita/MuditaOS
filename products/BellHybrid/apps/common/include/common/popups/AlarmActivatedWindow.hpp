// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/popups/presenter/PowerOffPresenter.hpp>
#include <apps-common/windows/Dialog.hpp>
#include <AsyncTask.hpp>

namespace gui
{
    class AlarmActivatedWindow : public Dialog, public app::AsyncCallbackReceiver
    {
      public:
        explicit AlarmActivatedWindow(app::ApplicationCommon *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        bool onAlarmResponseMessage(sys::ResponseMessage *response, ShowMode mode);
    };
} /* namespace gui */
