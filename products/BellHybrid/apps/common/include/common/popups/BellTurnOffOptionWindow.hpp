// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <common/options/BellShortOptionWindow.hpp>
#include <Timers/TimerHandle.hpp>

namespace gui
{
    constexpr std::chrono::seconds turnOffPopupTimeout{10};
    constexpr std::chrono::milliseconds defaultWindowTimeout{0};

    class BellTurnOffOptionWindow : public BellShortOptionWindow
    {
      public:
        static constexpr auto defaultName = "BellTurnOffOptionWindow";
        explicit BellTurnOffOptionWindow(app::ApplicationCommon *app,
                                         std::chrono::milliseconds duration = defaultWindowTimeout,
                                         const char *name                   = defaultName);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void onClose(CloseReason reason) override;

      private:
        std::list<Option> settingsOptionsList();
        sys::TimerHandle popupTimer;
        const UTF8 yesStr;
        const UTF8 noStr;
    };
} /* namespace gui */
