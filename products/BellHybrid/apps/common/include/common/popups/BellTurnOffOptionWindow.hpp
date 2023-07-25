// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/options/BellShortOptionWindow.hpp>
#include <Timers/TimerHandle.hpp>

namespace gui
{
    class BellTurnOffOptionWindow : public BellShortOptionWindow
    {
      public:
        static constexpr auto defaultName = "BellTurnOffOptionWindow";
        explicit BellTurnOffOptionWindow(app::ApplicationCommon *app, const char *name = defaultName);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;

      private:
        std::list<Option> settingsOptionsList();
        sys::TimerHandle popupTimer;
        const UTF8 yesStr;
        const UTF8 noStr;
    };
} /* namespace gui */
