// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>

namespace gui
{
    class Icon;

    class DeadBatteryWindow : public gui::AppWindow
    {
      public:
        static constexpr auto defaultName = "DeadBatteryWindow";

        DeadBatteryWindow(app::ApplicationCommon *app, const std::string &name = defaultName);

      private:
        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
    };

} // namespace gui
