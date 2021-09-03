// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellHomeScreenWindow.hpp"
#include "data/BellMainStyle.hpp"

#include <application-bell-main/ApplicationBellMain.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/actions/AlarmRingingData.hpp>
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/TextFixedSize.hpp>
#include <gui/widgets/Style.hpp>
#include <i18n/i18n.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-time/api/TimeSettingsApi.hpp>
#include <time/time_constants.hpp>
#include <widgets/AlarmSetSpinner.hpp>
#include <widgets/TimeSetFmtSpinner.hpp>
#include <widgets/TimeSetSpinner.hpp>

#include <chrono>

namespace
{
    using utils::time::Locale;
    void increaseHour(struct tm &tm)
    {
        if (tm.tm_hour >= Locale::max_hour_24H_mode) {
            tm.tm_hour = 0;
            tm.tm_min  = 0;
        }
        else {
            tm.tm_hour++;
            tm.tm_min = 0;
        }
    }

    void decreaseHour(struct tm &tm)
    {
        if (tm.tm_hour <= Locale::min_hour_24H_mode) {
            tm.tm_hour = Locale::max_hour_24H_mode;
            tm.tm_min  = Locale::max_minutes;
        }
        else {
            tm.tm_hour--;
            tm.tm_min = utils::time::Locale::max_minutes;
        }
    }

    void handleMinuteIncrease(struct tm &tm)
    {
        if (tm.tm_min >= Locale::max_minutes) {
            increaseHour(tm);
        }
        else {
            tm.tm_min++;
        }
    }

    void handleMinuteDecrease(struct tm &tm)
    {
        if (tm.tm_min <= 0) {
            decreaseHour(tm);
        }
        else {
            tm.tm_min--;
        }
    }
} // namespace

namespace gui
{
    BellHomeScreenWindow::BellHomeScreenWindow(app::Application *app,
                                               std::unique_ptr<app::home_screen::AbstractPresenter> presenter)
        : AppWindow(app, name::window::main_window), presenter{std::move(presenter)}
    {
        buildInterface();
        this->presenter->attach(this);
        this->presenter->createData();
        this->presenter->onBeforeShow();
    }

    void BellHomeScreenWindow::buildInterface()
    {
        using namespace bellMainStyle;
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        body = new BellBaseLayout(this, 0, 0, style::window_width, style::window_height, false);

        alarm = new AlarmSetSpinner(body->firstBox);
        alarm->setMinimumSize(mainWindow::alarmSetSpinner::width, mainWindow::alarmSetSpinner::height);
        alarm->setFont(mainWindow::alarmSetSpinner::font);
        alarm->setEditMode(EditMode::Browse);
        alarm->setAlarmStatus(AlarmSetSpinner::Status::DEACTIVATED);
        alarm->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        time = new TimeSetFmtSpinner(body->centerBox);
        time->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        time->setFont(bellMainStyle::mainWindow::time::font);
        time->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        time->setEditMode(EditMode::Browse);
        time->setFont(mainWindow::time::font);
        time->activeItem = false;

        bottomText = new TextFixedSize(body->lastBox, 0, 0, 0, 0);
        bottomText->setMinimumSize(mainWindow::bottomDescription::width, mainWindow::bottomDescription::height);
        bottomText->setFont(mainWindow::bottomDescription::font);
        bottomText->setEdges(RectangleEdge::None);
        bottomText->activeItem = false;
        bottomText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        bottomText->drawUnderline(false);
        bottomText->setLines(2);

        body->resizeItems();
        body->firstBox->resizeItems();
        body->centerBox->resizeItems();
        body->lastBox->resizeItems();
    }

    void BellHomeScreenWindow::setAlarmTriggered()
    {
        alarm->setAlarmStatus(AlarmSetSpinner::Status::RINGING);
    }

    void BellHomeScreenWindow::setAlarmSnoozed()
    {
        alarm->setAlarmStatus(AlarmSetSpinner::Status::SNOOZE);
    }

    void BellHomeScreenWindow::setAlarmActive(bool val)
    {
        if (val) {
            alarm->setAlarmStatus(AlarmSetSpinner::Status::ACTIVATED);
        }
        else {
            alarm->setAlarmStatus(AlarmSetSpinner::Status::DEACTIVATED);
        }
    }

    void BellHomeScreenWindow::setAlarmVisible(bool val)
    {
        alarm->setVisible(val);
    }

    void BellHomeScreenWindow::setAlarmTimeVisible(bool val)
    {
        alarm->setAlarmTimeVisible(val);
    }

    void BellHomeScreenWindow::setTemperature(utils::temperature::Temperature newTemp)
    {
        bottomText->setText(utils::temperature::tempToStrDec(newTemp));
    }

    void BellHomeScreenWindow::setBottomDescription(const UTF8 &desc)
    {
        bottomText->setText(desc);
    }

    void BellHomeScreenWindow::setTime(std::time_t newTime)
    {
        time->setTime(newTime);
    }

    void BellHomeScreenWindow::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        time->setTimeFormat(fmt);
    }

    void BellHomeScreenWindow::setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        alarm->setTimeFormat(fmt);
    }

    void BellHomeScreenWindow::setAlarmEdit(bool val)
    {
        if (not val) {
            alarm->setEditMode(EditMode::Browse);
        }
        else {
            alarm->setEditMode(EditMode::Edit);
        };
    }

    void BellHomeScreenWindow::switchToMenu()
    {
        application->switchWindow(window::name::bell_main_menu, nullptr);
    }

    bool BellHomeScreenWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            return presenter->handleInputEvent(inputEvent);
        }
        return false;
    }

    void BellHomeScreenWindow::onBeforeShow(ShowMode, SwitchData *data)
    {
        presenter->onBeforeShow();
        const auto alarmRingingSwitchData = dynamic_cast<app::actions::AlarmRingingData *>(data);
        if (alarmRingingSwitchData != nullptr) {
            presenter->handleAlarmRingingEvent();
        }
    }

    std::time_t BellHomeScreenWindow::getAlarmTime() const
    {
        const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto alarmTime = alarm->getTime();
        if (alarmTime < now) {
            alarmTime += utils::time::secondsInDay;
        }
        return alarmTime;
    }

    void BellHomeScreenWindow::setAlarmTime(std::time_t newTime)
    {
        alarm->setTime(newTime);
    }

    bool BellHomeScreenWindow::updateTime()
    {
        if (presenter) {
            presenter->handleUpdateTimeEvent();
        }
        return true;
    }

    void BellHomeScreenWindow::incAlarmMinute()
    {
        const auto alarmTime = alarm->getTime();
        auto newTime         = std::localtime(&alarmTime);
        handleMinuteIncrease(*newTime);
        alarm->setTime(std::mktime(newTime));
    }

    void BellHomeScreenWindow::decAlarmMinute()
    {
        const auto alarmTime = alarm->getTime();
        auto newTime         = std::localtime(&alarmTime);
        handleMinuteDecrease(*newTime);
        alarm->setTime(std::mktime(newTime));
    }
    bool BellHomeScreenWindow::onDatabaseMessage(sys::Message *msg)
    {
        auto *msgNotification = dynamic_cast<db::NotificationMessage *>(msg);
        if (msgNotification != nullptr) {
            presenter->onDatabaseMessage(msgNotification);
            return true;
        }
        return false;
    }
} // namespace gui
