// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>

#include <AppWindow.hpp>
#include <module-gui/gui/widgets/Image.hpp>

namespace gui
{
    class OnBoardingMainWindow : public gui::AppWindow
    {
      public:
        explicit OnBoardingMainWindow(app::Application *app);

        bool onInput(const gui::InputEvent &inputEvent) override;

        void buildInterface() override;
        top_bar::Configuration configureTopBar(top_bar::Configuration appConfiguration) override;
    };
} // namespace gui
