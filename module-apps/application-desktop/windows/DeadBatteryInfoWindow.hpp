// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/popups/WindowWithTimer.hpp>
#include <Icon.hpp>

namespace gui
{
    class DeadBatteryInfoWindow : public WindowWithTimer
    {
      public:
        DeadBatteryInfoWindow(app::ApplicationCommon *app, const std::string &name);
        ~DeadBatteryInfoWindow();

        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

      private:
        Icon *icon = nullptr;
    };
} // namespace gui
