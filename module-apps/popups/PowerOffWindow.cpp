// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerOffWindow.hpp"

#include <log/log.hpp>
#include <messages/DialogMetadataMessage.hpp>

namespace gui
{
    PowerOffWindow::PowerOffWindow(app::Application *app, std::unique_ptr<PowerOffPresenter> &&presenter)
        : DialogYesNo(app, popup::window::power_off_window), presenter(std::move(presenter))
    {
        topBar->configure(std::move(configureTopBar(application->getTopBarConfiguration())));
    }

    top_bar::Configuration PowerOffWindow::configureTopBar(top_bar::Configuration appConfiguration)
    {
        appConfiguration.enable(top_bar::Indicator::Time);
        appConfiguration.disable(top_bar::Indicator::Lock);
        appConfiguration.disable(top_bar::Indicator::Battery);
        appConfiguration.disable(top_bar::Indicator::Signal);
        appConfiguration.disable(top_bar::Indicator::SimCard);
        return appConfiguration;
    }

    void PowerOffWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        DialogMetadata metadata;
        metadata.action = [=]() -> bool {
            LOG_INFO("User call close system");
            presenter->powerOff();
            return true;
        };
        metadata.title = utils::translate("app_desktop_poweroff_title");
        metadata.text  = utils::translate("app_desktop_poweroff_question");
        metadata.icon  = "turn_off_W_G";
        auto msg       = std::make_unique<DialogMetadataMessage>(std::move(metadata));
        DialogYesNo::onBeforeShow(mode, msg.get());

        bottomBar->setActive(BottomBar::Side::CENTER, false);
        bottomBar->setActive(BottomBar::Side::RIGHT, false);
    }

} /* namespace gui */
