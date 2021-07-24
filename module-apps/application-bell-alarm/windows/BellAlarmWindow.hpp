// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-bell-alarm/ApplicationBellAlarm.hpp>

#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class BellAlarmWindow : public AppWindow
    {
      public:
        explicit BellAlarmWindow(app::Application *app, std::string name = window::name::bellAlarm);

        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;
    };
} /* namespace gui */
