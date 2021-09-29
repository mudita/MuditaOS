// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellAlarmSetWindow.hpp"
#include "data/BellAlarmStyle.hpp"
#include <application-bell-alarm/ApplicationBellAlarm.hpp>

#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>

#include <common/TimeUtils.hpp>

namespace gui
{
    BellAlarmSetWindow::BellAlarmSetWindow(app::ApplicationCommon *app,
                                           std::shared_ptr<app::bell_alarm::BellAlarmSetContract::Presenter> presenter)
        : WindowWithTimer(app, gui::window::name::bellAlarmSet), presenter{std::move(presenter)}
    {
        buildInterface();
    }

    void BellAlarmSetWindow::buildInterface()
    {
        AppWindow::buildInterface();
        buildLayout();
        registerCallbacks();
    }

    void BellAlarmSetWindow::buildLayout()
    {
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        icon = new Icon(this, 0, 0, style::window_width, style::window_height, {}, {});
        icon->text->setFont(style::window::font::verybiglight);
    }

    void BellAlarmSetWindow::registerCallbacks()
    {
        timerCallback = [this](Item &, sys::Timer &timer) {
            presenter->activate();
            return true;
        };
    }

    void BellAlarmSetWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        WindowWithTimer::onBeforeShow(mode, data);

        if (presenter->isAlarmActive()) {
            icon->image->set("big_alarm_W_G");
            icon->text->setRichText(utils::time::getBottomDescription(
                utils::time::calculateTimeDifference(presenter->getAlarmTime(), utils::time::getCurrentTime())));
        }
        else {
            icon->image->set("big_no-alarm_W_G");
            icon->text->setRichText(utils::translate("app_bell_alarm_set_not_active"));
        }
    }

    bool BellAlarmSetWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            detachTimerIfExists();
            presenter->activate();
            return true;
        }
        return false;
    }
} // namespace gui
