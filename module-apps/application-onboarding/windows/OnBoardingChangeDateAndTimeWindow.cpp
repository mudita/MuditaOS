// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingChangeDateAndTimeWindow.hpp"
#include <module-apps/application-onboarding/ApplicationOnBoarding.hpp>
#include <InputEvent.hpp>

namespace app::onBoarding
{
    OnBoardingChangeDateAndTimeWindow::OnBoardingChangeDateAndTimeWindow(app::Application *app)
        : ChangeDateAndTimeWindow(app, gui::window::name::onBoarding_change_date_and_time)
    {}

    bool OnBoardingChangeDateAndTimeWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isKeyRelease(gui::KeyCode::KEY_ENTER)) {
            auto ret = ChangeDateAndTimeWindow::onInput(inputEvent);
            application->returnToPreviousWindow();
            return ret;
        }

        return ChangeDateAndTimeWindow::onInput(inputEvent);
    }

} // namespace app::onBoarding
