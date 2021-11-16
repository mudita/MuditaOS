// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellHomeScreenWindow.hpp"
#include "data/BellMainStyle.hpp"
#include "widgets/SnoozeTimer.hpp"
#include "BellBatteryStatusWindow.hpp"
#include "ProductConfig.hpp"

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
    inline constexpr auto snoozeTimerName = "SnoozeTimer";
    inline constexpr std::chrono::seconds timerTick{1};
} // namespace

namespace gui
{
    BellHomeScreenWindow::BellHomeScreenWindow(app::ApplicationCommon *app,
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
        navBar->setVisible(false);

        body = new BellBaseLayout(this, 0, 0, style::window_width, style::window_height, false);

        alarm = new AlarmSetSpinner(body->firstBox);
        alarm->setMinimumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::outer_layouts_h);
        alarm->setFont(mainWindow::alarmSetSpinner::font);
        alarm->setEditMode(EditMode::Browse);
        alarm->setAlarmStatus(AlarmSetSpinner::Status::DEACTIVATED);
        alarm->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        snoozeTimer = new SnoozeTimer(body->firstBox);
        snoozeTimer->setMinimumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::outer_layouts_h);
        snoozeTimer->setVisible(false);

        time = new TimeSetFmtSpinner(body->centerBox);
        time->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        time->setFont(bellMainStyle::mainWindow::time::font);
        time->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        time->setEditMode(EditMode::Browse);
        time->setFont(mainWindow::time::font);
        time->activeItem = false;

        bottomBox = new HBox(body->lastBox, 0, 0, 0, 0);
        bottomBox->setMinimumSize(style::bell_base_layout::outer_layouts_w, style::bell_base_layout::outer_layouts_h);
        bottomBox->setEdges(RectangleEdge::None);
        bottomBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        battery = new BellBattery(bottomBox, 0, 0, 0, 0);
        battery->setMinimumSize(battery::battery_widget_w, battery::battery_widget_h);
        battery->setEdges(RectangleEdge::None);
        battery->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
        battery->setVisible(false);

        bottomText = new TextFixedSize(bottomBox, 0, 0, 0, 0);
        bottomText->setMaximumSize(style::bell_home_screen::bottom_desc_w, style::bell_home_screen::bottom_desc_h);
        bottomText->setFont(mainWindow::bottomDescription::font_small);
        bottomText->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Top));
        bottomText->setEdges(RectangleEdge::None);
        bottomText->activeItem = false;
        bottomText->drawUnderline(false);

        body->resize();

        auto timer = std::make_unique<app::ProgressTimerWithSnoozeTimer>(
            application, *this, snoozeTimerName, timerTick, app::ProgressCountdownMode::Increasing);
        timer->attach(snoozeTimer);
        presenter->setSnoozeTimer(std::move(timer));
    }

    void BellHomeScreenWindow::setAlarmTriggered()
    {
        alarm->setAlarmStatus(AlarmSetSpinner::Status::RINGING);
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

    void BellHomeScreenWindow::setHeaderViewMode(app::home_screen::HeaderViewMode mode)
    {
        switch (mode) {
        case app::home_screen::HeaderViewMode::Empty:
            alarm->setVisible(false);
            alarm->setAlarmTimeVisible(false);
            snoozeTimer->setVisible(false);
            alarm->informContentChanged();
            break;
        case app::home_screen::HeaderViewMode::AlarmIconAndTime:
            alarm->setVisible(true);
            alarm->setAlarmTimeVisible(true);
            snoozeTimer->setVisible(false);
            alarm->informContentChanged();
            break;
        case app::home_screen::HeaderViewMode::AlarmIcon:
            alarm->setVisible(true);
            alarm->setAlarmTimeVisible(false);
            snoozeTimer->setVisible(false);
            alarm->informContentChanged();
            break;
        case app::home_screen::HeaderViewMode::SnoozeCountdown:
            alarm->setVisible(false);
            alarm->setAlarmTimeVisible(false);
            snoozeTimer->setVisible(true);
            snoozeTimer->informContentChanged();
            break;
        }
    }

    void BellHomeScreenWindow::setTemperature(utils::temperature::Temperature newTemp)
    {
#if CONFIG_ENABLE_TEMP == 1
        bottomText->setFont(bellMainStyle::mainWindow::bottomDescription::font_normal);
        bottomText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        bottomText->setText(utils::temperature::tempToStrDec(newTemp));
        bottomBox->resizeItems();
#else
        bottomText->setVisible(false);
        bottomBox->resizeItems();
#endif
    }

    void BellHomeScreenWindow::setBottomDescription(const UTF8 &desc)
    {
        battery->setVisible(false);
        bottomText->setVisible(true);
        bottomText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        bottomText->setFont(bellMainStyle::mainWindow::bottomDescription::font_small);
        bottomText->setRichText(desc);
        bottomBox->resizeItems();
    }

    void BellHomeScreenWindow::setBatteryLevelState(const Store::Battery &batteryContext)
    {
        battery->update(batteryContext);
        if (!battery->visible) {
            bottomText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        }
        else {
#if CONFIG_ENABLE_TEMP == 1
            bottomText->setAlignment(Alignment(Alignment::Horizontal::Right, Alignment::Vertical::Center));
            battery->setAlignment(Alignment(Alignment::Horizontal::Left, Alignment::Vertical::Center));
#else
            battery->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
#endif
        }
        bottomBox->resizeItems();
    }

    void BellHomeScreenWindow::setTime(std::time_t newTime)
    {
        time->setTime(newTime);
        time->setTimeFormatSpinnerVisibility(false);
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
        if (inputEvent.isShortRelease() || inputEvent.isLongRelease()) {
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
    void BellHomeScreenWindow::switchToBatteryStatus()
    {
        application->switchWindow(gui::BellBatteryStatusWindow::name,
                                  std::make_unique<gui::BellBatteryStatusWindow::Data>(presenter->getBatteryLvl(),
                                                                                       presenter->isBatteryCharging()));
    }

} // namespace gui
