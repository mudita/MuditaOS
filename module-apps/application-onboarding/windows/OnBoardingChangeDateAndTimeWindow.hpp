// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings-new/windows/ChangeDateAndTimeWindow.hpp>
#include <application-settings-new/models/DateAndTimeModel.hpp>

namespace app::onBoarding
{
    class OnBoardingChangeDateAndTimeWindow : public gui::ChangeDateAndTimeWindow
    {
      public:
        explicit OnBoardingChangeDateAndTimeWindow(app::Application *app);

        bool onInput(const gui::InputEvent &inputEvent) override;
    };
} // namespace app::onBoarding
