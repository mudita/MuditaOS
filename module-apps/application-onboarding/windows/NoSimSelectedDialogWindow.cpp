// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NoSimSelectedDialogWindow.hpp"

#include <application-onboarding/ApplicationOnBoarding.hpp>

#include <i18n/i18n.hpp>
#include <Style.hpp>

namespace app::onBoarding
{
    NoSimSelectedDialogWindow::NoSimSelectedDialogWindow(app::ApplicationCommon *app)
        : gui::Dialog(app, gui::window::name::onBoarding_no_sim_selected)
    {
        navBar->setText(gui::nav_bar::Side::Center, utils::translate(style::strings::common::ok));
        navBar->setActive(gui::nav_bar::Side::Right, true);

        applyToStatusBar([](gui::status_bar::Configuration configuration) {
            configuration.setIndicator(gui::status_bar::Indicator::SimCard, false);
            return configuration;
        });
    }

    bool NoSimSelectedDialogWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            if (inputEvent.is(gui::KeyCode::KEY_ENTER)) {
                application->getPhoneLockSubject().setPhoneLock();
            }
        }
        return AppWindow::onInput(inputEvent);
    }
} // namespace app::onBoarding
