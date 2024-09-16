// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "OnBoardingChangeDateAndTimeWindow.hpp"
#include <module-apps/application-onboarding/ApplicationOnBoarding.hpp>
#include <InputEvent.hpp>

namespace app::onBoarding
{
    OnBoardingChangeDateAndTimeWindow::OnBoardingChangeDateAndTimeWindow(app::ApplicationCommon *app)
        : ChangeDateAndTimeWindow(app, gui::window::name::onBoarding_change_date_and_time)
    {}

    bool OnBoardingChangeDateAndTimeWindow::onInput(const gui::InputEvent &inputEvent)
    {
        return ChangeDateAndTimeWindow::onInput(inputEvent);
    }
} // namespace app::onBoarding
