// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/system/DateAndTimeMainWindow.hpp>

namespace app::onBoarding
{
    class OnBoardingDateAndTimeWindow : public gui::DateAndTimeMainWindow
    {
      public:
        explicit OnBoardingDateAndTimeWindow(app::Application *app);
        ~OnBoardingDateAndTimeWindow();

      protected:
        bool bottomBarCallback(Item &item) override;

      private:
        void onBeforeShow(gui::ShowMode mode, gui::SwitchData *data) override;
        bool onInput(const gui::InputEvent &inputEvent) override;
    };
} // namespace app::onBoarding
