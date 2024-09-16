// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "BellAlarmSetWindow.hpp"
#include "data/BellAlarmStyle.hpp"
#include <common/data/StyleCommon.hpp>
#include <application-bell-alarm/ApplicationBellAlarm.hpp>

#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>

#include <common/TimeUtils.hpp>

namespace
{
    constexpr std::chrono::seconds screenTimeout{5};
} // namespace

namespace gui
{
    BellAlarmSetWindow::BellAlarmSetWindow(app::ApplicationCommon *app,
                                           std::shared_ptr<app::bell_alarm::BellAlarmSetContract::Presenter> presenter)
        : WindowWithTimer(app, gui::window::name::bellAlarmSet, alarmSummaryDisplayDuration), presenter{
                                                                                                  std::move(presenter)}
    {
        buildInterface();
    }

    void BellAlarmSetWindow::buildInterface()
    {
        AppWindow::buildInterface();
        buildLayout();
        resetTimer(screenTimeout);
    }

    void BellAlarmSetWindow::buildLayout()
    {
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        switch (presenter->getLayout()) {
        case app::bell_alarm::Layout::AlarmInactive:
            buildAlarmInactiveLayout();
            break;
        case app::bell_alarm::Layout::LowBatteryInfo:
            buildLowBatteryLayout();
            break;
        case app::bell_alarm::Layout::AlarmInfo:
        case app::bell_alarm::Layout::Undefined:
            buildAlarmInfoLayout();
            break;
        }
    }

    void BellAlarmSetWindow::buildLowBatteryLayout()
    {
        icon = new Icon(this,
                        0,
                        0,
                        style::window_width,
                        style::window_height,
                        "bell_status_battery_lvl0",
                        {},
                        gui::ImageTypeSpecifier::W_G);
        icon->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        icon->image->setMargins({0, gui::bell_style::warning_icon_top_margin, 0, 0});
        icon->text->setMaximumWidth(gui::bell_style::warning_text_width);
        icon->text->setFont(style::window::font::verybiglight);
        icon->text->setRichText(utils::translate("app_bell_alarm_low_battery_info"));

        timerCallback = [this](Item &, sys::Timer &timer) {
            lowBatteryInfoHandled();
            return true;
        };
    }

    void BellAlarmSetWindow::buildAlarmInfoLayout()
    {
        icon = new Icon(this, 0, 0, style::window_width, style::window_height, "big_alarm_W_G", {});
        icon->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        icon->image->setMargins(
            {0, bell_alarm_style::icon::imageTopMargin, 0, bell_alarm_style::icon::imageBottomMargin});
        icon->text->setFont(style::window::font::verybiglight);
        icon->text->setRichText(utils::time::getBottomDescription(
            utils::time::calculateMinutesDifference(presenter->getAlarmTime(), utils::time::getCurrentTime())));

        timerCallback = [this](Item &, sys::Timer &timer) {
            presenter->activate();
            return true;
        };
    }

    void BellAlarmSetWindow::buildAlarmInactiveLayout()
    {
        icon = new Icon(this, 0, 0, style::window_width, style::window_height, "big_no-alarm_W_G", {});
        icon->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        icon->image->setMargins(
            {0, bell_alarm_style::icon::imageTopMargin, 0, bell_alarm_style::icon::imageBottomMargin});
        icon->text->setFont(style::window::font::verybiglight);
        icon->text->setRichText(utils::translate("app_bell_alarm_set_not_active"));

        timerCallback = [this](Item &, sys::Timer &timer) {
            presenter->activate();
            return true;
        };
    }

    void BellAlarmSetWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellAlarmSetWindow::lowBatteryInfoHandled()
    {
        presenter->lowBatteryInfoHandled();
        rebuild();
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    bool BellAlarmSetWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            detachTimerIfExists();
            if (presenter->getLayout() == app::bell_alarm::Layout::LowBatteryInfo) {
                lowBatteryInfoHandled();
            }
            else {
                presenter->activate();
            }
            return true;
        }
        return false;
    }
} // namespace gui
