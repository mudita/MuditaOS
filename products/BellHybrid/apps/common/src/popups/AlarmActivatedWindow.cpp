// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md


#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <common/TimeUtils.hpp>
#include <common/popups/AlarmActivatedWindow.hpp>
#include <popups/Popups.hpp>

namespace gui
{
    AlarmActivatedWindow::AlarmActivatedWindow(app::ApplicationCommon *app)
        : Dialog(app, popup::window::alarm_activated_window)
    {}

    void AlarmActivatedWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        const auto alarmTime = utils::time::getCurrentTime(); // TODO: Get alarm time from db
        auto msg             = std::make_unique<DialogMetadataMessage>(
            gui::DialogMetadata{.title = "",
                                .icon  = "bell_alarm_activated",
                                .text  = utils::time::getBottomDescription(
                                    utils::time::calculateTimeDifference(alarmTime, utils::time::getCurrentTime()))});
        Dialog::onBeforeShow(mode, msg.get());

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setActive(BottomBar::Side::RIGHT, false);
    }
} /* namespace gui */
