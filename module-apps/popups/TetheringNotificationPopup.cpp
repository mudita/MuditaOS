// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TetheringNotificationPopup.hpp"
#include <module-apps/application-desktop/ApplicationDesktop.hpp>

namespace gui
{
    TetheringNotificationPopup::TetheringNotificationPopup(app::Application *app, const std::string &name)
        : gui::DialogConfirm(app, name)
    {
        topBar->configure(configureTopBar(application->getTopBarConfiguration()));
    }

    gui::top_bar::Configuration TetheringNotificationPopup::configureTopBar(
        gui::top_bar::Configuration appConfiguration)
    {
        appConfiguration.setIndicator(gui::top_bar::Indicator::Time, false);
        appConfiguration.setIndicator(gui::top_bar::Indicator::Battery, true);
        appConfiguration.setIndicator(gui::top_bar::Indicator::SimCard, false);
        appConfiguration.setIndicator(gui::top_bar::Indicator::Signal, true);
        return appConfiguration;
    }
} // namespace gui
