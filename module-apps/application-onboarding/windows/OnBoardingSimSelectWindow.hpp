// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>
#include <Text.hpp>

namespace app::onBoarding
{
    class OnBoardingSimSelectWindow : public gui::BaseSettingsWindow
    {
      public:
        explicit OnBoardingSimSelectWindow(app::Application *app, std::string name);
        void buildInterface() override;

      private:
        gui::Text *descriptionText = nullptr;

        auto buildOptionsList() -> std::list<gui::Option> override;
        bool onInput(const gui::InputEvent &inputEvent) override;
    };
} // namespace app::onBoarding
