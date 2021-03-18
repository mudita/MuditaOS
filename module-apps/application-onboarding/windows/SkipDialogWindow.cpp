// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SkipDialogWindow.hpp"
#include <module-apps/application-onboarding/ApplicationOnBoarding.hpp>

namespace app::onBoarding
{
    SkipDialogWindow::SkipDialogWindow(app::Application *app)
        : gui::DialogYesNo(app, gui::window::name::onBoarding_skip)
    {
        topBar->configure(configureTopBar(application->getTopBarConfiguration()));
    }

    gui::top_bar::Configuration SkipDialogWindow::configureTopBar(gui::top_bar::Configuration appConfiguration)
    {
        appConfiguration.setIndicator(gui::top_bar::Indicator::Time, true);
        appConfiguration.setIndicator(gui::top_bar::Indicator::Battery, false);
        appConfiguration.setIndicator(gui::top_bar::Indicator::SimCard, false);
        appConfiguration.setIndicator(gui::top_bar::Indicator::Signal, false);
        return appConfiguration;
    }
} // namespace app::onBoarding
