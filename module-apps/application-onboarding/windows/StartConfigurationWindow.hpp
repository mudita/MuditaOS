// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>

#include <AppWindow.hpp>
#include <module-gui/gui/widgets/Icon.hpp>

namespace gui
{
    class StartConfigurationWindow : public gui::AppWindow
    {
      public:
        explicit StartConfigurationWindow(app::Application *app);

        bool onInput(const gui::InputEvent &inputEvent) override;

        void buildInterface() override;
    };
} // namespace gui
