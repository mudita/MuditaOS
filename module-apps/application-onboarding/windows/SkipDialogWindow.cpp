// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SkipDialogWindow.hpp"
#include <module-apps/application-onboarding/ApplicationOnBoarding.hpp>

namespace app::onBoarding
{
    SkipDialogWindow::SkipDialogWindow(app::ApplicationCommon *app)
        : gui::DialogYesNo(app, gui::window::name::onBoarding_skip)
    {
        applyToStatusBar([](gui::status_bar::Configuration configuration) {
            configuration.setIndicator(gui::status_bar::Indicator::SimCard, false);
            return configuration;
        });
    }
} // namespace app::onBoarding
