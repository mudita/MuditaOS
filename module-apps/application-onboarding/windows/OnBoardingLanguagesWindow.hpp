// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/system/LanguagesWindow.hpp>

namespace app::onBoarding
{
    class OnBoardingLanguagesWindow : public gui::LanguagesWindow
    {
      public:
        explicit OnBoardingLanguagesWindow(app::ApplicationCommon *app, const std::string &name);

      private:
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
    };
} // namespace app::onBoarding
