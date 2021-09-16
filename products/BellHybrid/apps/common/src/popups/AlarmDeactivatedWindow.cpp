// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/popups/AlarmDeactivatedWindow.hpp>

#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/popups/Popups.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    AlarmDeactivatedWindow::AlarmDeactivatedWindow(app::ApplicationCommon *app)
        : Dialog(app, popup::window::alarm_deactivated_window)
    {}

    void AlarmDeactivatedWindow::onBeforeShow(ShowMode mode, [[maybe_unused]] SwitchData *data)
    {
        auto msg = std::make_unique<DialogMetadataMessage>(DialogMetadata{
            .title = "", .icon = "bell_alarm_deactivated", .text = utils::translate("app_bell_alarm_deactivated")});
        Dialog::onBeforeShow(mode, msg.get());

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setActive(BottomBar::Side::RIGHT, false);
    }
} /* namespace gui */
