// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>

#include <AppWindow.hpp>
#include <module-gui/gui/widgets/Icon.hpp>

namespace app::onBoarding
{
    class StartConfigurationWindow : public gui::AppWindow
    {
      public:
        explicit StartConfigurationWindow(app::ApplicationCommon *app);

        gui::status_bar::Configuration configureStatusBar(gui::status_bar::Configuration appConfiguration) override;
        bool onInput(const gui::InputEvent &inputEvent) override;

        void buildInterface() override;
    };
} // namespace app::onBoarding
