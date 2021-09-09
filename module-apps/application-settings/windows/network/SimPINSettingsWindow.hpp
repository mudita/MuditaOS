// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace gui
{
    class SimPINSettingsWindow : public BaseSettingsWindow
    {
      public:
        explicit SimPINSettingsWindow(app::ApplicationCommon *app);

      private:
        auto buildOptionsList() -> std::list<Option> override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void changePinState(bool &currentState);

        bool pinIsOn = false;
    };
} // namespace gui
