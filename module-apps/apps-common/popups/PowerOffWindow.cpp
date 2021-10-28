// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerOffWindow.hpp"
#include "Popups.hpp"

#include <log/log.hpp>
#include <messages/DialogMetadataMessage.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    PowerOffWindow::PowerOffWindow(app::ApplicationCommon *app, std::unique_ptr<PowerOffPresenter> &&presenter)
        : DialogYesNo(app, popup::window::power_off_window), presenter(std::move(presenter))
    {}

    void PowerOffWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        DialogMetadata metadata;
        metadata.action = [=]() -> bool {
            LOG_INFO("User call close system");
            preventsAutoLock = true;
            presenter->powerOff();
            return true;
        };
        metadata.title = utils::translate("app_desktop_poweroff_title");
        metadata.text  = utils::translate("app_desktop_poweroff_question");
        metadata.icon  = "turn_off_W_G";
        auto msg       = std::make_unique<DialogMetadataMessage>(std::move(metadata));
        DialogYesNo::onBeforeShow(mode, msg.get());
    }

} /* namespace gui */
