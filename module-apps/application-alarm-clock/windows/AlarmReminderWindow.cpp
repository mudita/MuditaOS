// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmReminderWindow.hpp"
#include "application-alarm-clock/widgets/AlarmClockStyle.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include <gui/widgets/Window.hpp>
#include <time/time_conversion.hpp>
#include <service-appmgr/Controller.hpp>
#include <module-services/service-audio/service-audio/AudioServiceAPI.hpp>

namespace app::alarmClock
{
    constexpr static const int reminderLifeDuration = 900000; // 15min
    constexpr static const int defaultDuration      = 60000;  // 1min

    AlarmReminderWindow::AlarmReminderWindow(app::Application *app,
                                             std::unique_ptr<AlarmReminderWindowContract::Presenter> &&windowPresenter)
        : AppWindow(app, style::alarmClock::window::name::alarmReminder), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();

        reminderTimer = std::make_unique<sys::Timer>(
            "AlarmClockReminderTimer", app, reminderLifeDuration, sys::Timer::Type::SingleShot);
        musicTimer =
            std::make_unique<sys::Timer>("AlarmClockMusicTimer", app, defaultDuration, sys::Timer::Type::Periodic);
        delayTimer = std::make_unique<sys::Timer>("AlarmDelayTimer", app, defaultDuration, sys::Timer::Type::Periodic);
    }

    AlarmReminderWindow::~AlarmReminderWindow()
    {
        destroyInterface();
    }

    void AlarmReminderWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void AlarmReminderWindow::buildInterface()
    {
        AppWindow::buildInterface();

        topBar->setActive(gui::TopBar::Elements::BATTERY, true);
        topBar->setActive(gui::TopBar::Elements::SIGNAL, true);
        topBar->setActive(gui::TopBar::Elements::TIME, true);

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::stop));

        namespace alarmReminder = style::alarmClock::window::alarmReminder;
        const uint32_t w = this->getWidth() - style::window::default_left_margin - style::window::default_right_margin;
        const uint32_t h = this->getHeight() - bottomBar->getHeight();
        body             = new gui::VBox(this, style::window::default_left_margin, alarmReminder::marginTop, w, h);
        body->setEdges(gui::RectangleEdge::None);

        hBox = new gui::HBox(body, 0, 0, w, alarmReminder::hBoxHeight);
        hBox->setAlignment(gui::Alignment::Horizontal::Center);
        hBox->setEdges(gui::RectangleEdge::None);

        leftImage = new gui::Image(hBox, 0, 0, 0, 0, "circle_left");
        leftImage->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center));

        vBox = new gui::VBox(hBox, 0, 0, alarmReminder::vBoxWidth, alarmReminder::hBoxHeight);
        vBox->setEdges(gui::RectangleEdge::None);

        timeLabel = new gui::Label(vBox, 0, 0, alarmReminder::vBoxWidth, alarmReminder::timeLabelHeight);
        timeLabel->setMargins(gui::Margins(0, alarmReminder::marginTop, 0, alarmReminder::timeLabelMarginBot));
        timeLabel->setFont(style::window::font::largelight);
        timeLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        timeLabel->setEdges(gui::RectangleEdge::None);

        alarmLabel = new gui::Label(vBox, 0, 0, alarmReminder::vBoxWidth, alarmReminder::alarmLabelHeight);
        alarmLabel->setFont(style::window::font::mediumlight);
        alarmLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        alarmLabel->setEdges(gui::RectangleEdge::None);
        alarmLabel->setText(utils::localize.get("app_alarm_clock_alarm"));

        rightImage = new gui::Image(hBox, 0, 0, 0, 0, "circle_right");
        rightImage->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center));

        snoozeVBox = new gui::VBox(body, 0, 0, alarmReminder::snoozeVBox, alarmReminder::snoozeVBox);
        snoozeVBox->setMargins(gui::Margins(0, alarmReminder::snoozeVBoxMarginTop, 0, 0));
        snoozeVBox->setAlignment(gui::Alignment::Horizontal::Center);
        snoozeVBox->setPenWidth(style::window::default_border_focus_w);
        snoozeVBox->setEdges(gui::RectangleEdge::Bottom | gui::RectangleEdge::Top);

        snoozeImage = new gui::Image(snoozeVBox, 0, 0, 0, 0, "snooze_icon");
        snoozeImage->setMargins(
            gui::Margins(0, alarmReminder::snoozeImageMarginTop, 0, alarmReminder::snoozeImageMarginBot));
        snoozeImage->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        snoozeLabel = new gui::Label(snoozeVBox, 0, 0, alarmReminder::snoozeVBox, alarmReminder::snoozeLabelHeight);
        snoozeLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        snoozeLabel->setFont(style::window::font::verysmall);
        snoozeLabel->setEdges(gui::RectangleEdge::None);
        snoozeLabel->setText(utils::localize.get("app_alarm_clock_snooze_uppercase"));

        setFocusItem(body);
    }

    void AlarmReminderWindow::destroyInterface()
    {
        destroyTimers();
        erase();
    }

    void AlarmReminderWindow::startTimers()
    {
        elapsedMinutes = 0;
        reminderTimer->connect([=](sys::Timer &) { closeReminderCallback(); });
        reminderTimer->reload();
        delayTimer->connect([=](sys::Timer &) { countElapsedMinutes(); });
        delayTimer->reload();
    }

    void AlarmReminderWindow::startMusicTimer()
    {
        musicTimer->connect([=](sys::Timer &) { loopMusic(); });
        musicTimer->reload();
    }

    void AlarmReminderWindow::destroyTimers()
    {
        reminderTimer->stop();
        delayTimer->stop();
        musicTimer->stop();
    }

    auto AlarmReminderWindow::handleSwitchData(gui::SwitchData *data) -> bool
    {
        startTimers();

        if (data == nullptr) {
            return false;
        }

        auto *item = dynamic_cast<AlarmRecordData *>(data);
        if (item == nullptr) {
            return false;
        }

        alarmRecord = item->getData();
        if (previousAlarmRecord != nullptr) {
            LOG_DEBUG("New alarm covered the old one, automatic snooze applying");
            presenter->update(*previousAlarmRecord, UserAction::Snooze, previousElapsedMinutes);
            previousElapsedMinutes = 0;
        }
        previousAlarmRecord = alarmRecord;
        timeLabel->setText(TimePointToLocalizedTimeString(alarmRecord->time, "%I:%0M"));
        auto fileTags = AudioServiceAPI::GetFileTags(application, alarmRecord->path);

        if (fileTags != std::nullopt) {
            /*
            musicTimer->setInterval(fileTags->duration_sec * 1000);
            startMusicTimer();
            AudioServiceAPI::PlaybackStart(
                        application, audio::PlaybackType::Multimedia, alarmRecord->path);*/
        }
        return true;
    }

    bool AlarmReminderWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.is(gui::KeyCode::KEY_ENTER)) {
            presenter->update(*alarmRecord, UserAction::Snooze, elapsedMinutes);
            closeReminder();
            return true;
        }

        if (inputEvent.is(gui::KeyCode::KEY_RF)) {
            presenter->update(*alarmRecord, UserAction::TurnOff, 0);
            closeReminder();
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

    void AlarmReminderWindow::closeReminder()
    {
        LOG_DEBUG("Switch to home window");

        destroyTimers();
        // AudioServiceAPI::StopAll(application);
        app::manager::Controller::sendAction(application, app::manager::actions::Home);
    }

    void AlarmReminderWindow::loopMusic()
    {
        AudioServiceAPI::PlaybackStart(application, audio::PlaybackType::Multimedia, alarmRecord->path);
    }

    void AlarmReminderWindow::countElapsedMinutes()
    {
        ++elapsedMinutes;
        previousElapsedMinutes = elapsedMinutes;
    }

    void AlarmReminderWindow::closeReminderCallback()
    {
        presenter->update(*alarmRecord, UserAction::Snooze, elapsedMinutes);
        closeReminder();
    }

} // namespace app::alarmClock
