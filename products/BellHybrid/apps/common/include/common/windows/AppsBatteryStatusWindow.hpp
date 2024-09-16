// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "BellBatteryStatusWindow.hpp"

namespace gui
{
    class AppsBatteryStatusWindow : public BellBatteryStatusWindow
    {
      public:
        AppsBatteryStatusWindow(app::ApplicationCommon *app, const std::string &name);

      private:
        bool onInput(const InputEvent &inputEvent) override;
        bool handleSwitchData(SwitchData *data) override;

        std::function<void()> returnCallback;
    };

} // namespace gui
