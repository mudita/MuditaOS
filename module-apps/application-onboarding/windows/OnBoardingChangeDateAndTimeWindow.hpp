// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/system/ChangeDateAndTimeWindow.hpp>

namespace app::onBoarding
{
    class OnBoardingChangeDateAndTimeWindow : public gui::ChangeDateAndTimeWindow
    {
      public:
        explicit OnBoardingChangeDateAndTimeWindow(app::ApplicationCommon *app);

        bool onInput(const gui::InputEvent &inputEvent) override;
    };
} // namespace app::onBoarding
