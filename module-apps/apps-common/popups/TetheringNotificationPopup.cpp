// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TetheringNotificationPopup.hpp"

#include <ApplicationCommon.hpp>

namespace gui
{
    TetheringNotificationPopup::TetheringNotificationPopup(app::ApplicationCommon *app, const std::string &name)
        : gui::DialogConfirm(app, name)
    {
        statusBar->configure(configureStatusBar(application->getStatusBarConfiguration()));
    }

    gui::status_bar::Configuration TetheringNotificationPopup::configureStatusBar(
        gui::status_bar::Configuration appConfiguration)
    {
        appConfiguration.setIndicator(gui::status_bar::Indicator::Time, false);
        appConfiguration.setIndicator(gui::status_bar::Indicator::Battery, true);
        appConfiguration.setIndicator(gui::status_bar::Indicator::SimCard, false);
        appConfiguration.setIndicator(gui::status_bar::Indicator::Signal, true);
        return appConfiguration;
    }
} // namespace gui
