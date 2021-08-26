// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-bell-settings/ApplicationBellSettings.hpp"

#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    class Icon;

    class BellSettingsFinishedWindow : public WindowWithTimer
    {
      public:
        BellSettingsFinishedWindow(app::Application *app, const std::string &name = window::name::bellSettingsFinished);

      private:
        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        Icon *icon{};

        std::string message;
    };

} // namespace gui
