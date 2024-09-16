// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class Icon;

    class BellBatteryShutdownWindow : public gui::AppWindow
    {
      public:
        static constexpr auto defaultName = "BellBatteryShutdown";

        explicit BellBatteryShutdownWindow(app::ApplicationCommon *app, const std::string &name = defaultName);

      private:
        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
    };
} // namespace gui
