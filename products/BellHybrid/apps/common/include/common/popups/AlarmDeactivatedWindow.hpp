// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/windows/Dialog.hpp>

namespace gui
{
    class AlarmDeactivatedWindow : public Dialog
    {
      public:
        explicit AlarmDeactivatedWindow(app::ApplicationCommon *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };
} /* namespace gui */
