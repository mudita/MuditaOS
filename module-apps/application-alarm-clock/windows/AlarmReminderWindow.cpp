// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmReminderWindow.hpp"
#include "application-alarm-clock/widgets/AlarmClockStyle.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include <gui/widgets/Window.hpp>
#include <time/time_conversion.hpp>
#include <service-appmgr/Controller.hpp>

namespace app::alarmClock
{
    AlarmReminderWindow::AlarmReminderWindow(app::Application *app,
                                             std::unique_ptr<AlarmReminderWindowContract::Presenter> &&windowPresenter)
        : AppWindow(app, style::alarmClock::window::name::alarmReminder), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    AlarmReminderWindow::~AlarmReminderWindow()
    {
        presenter->stopTimers();
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
        erase();
    }

    auto AlarmReminderWindow::handleSwitchData(gui::SwitchData *data) -> bool
    {
        if (data == nullptr) {
            return false;
        }

        auto item = dynamic_cast<AlarmRecordsData *>(data);
        if (item == nullptr) {
            return false;
        }
        alarmRecords = item->getRecords();
        if (!previousAlarmRecords.empty()) {
            if (previousAlarmRecords.front().ID == alarmRecords.front().ID) {
                LOG_DEBUG("Duplicated data received, return");
                return false;
            }
            LOG_DEBUG("New alarm covered the old one, automatic snooze applying");
            presenter->updatePreviousRecords(previousAlarmRecords);
        }
        presenter->startTimers(getCallback());
        previousAlarmRecords = alarmRecords;
        displayAlarm();
        return true;
    }

    void AlarmReminderWindow::displayAlarm()
    {
        auto rec = alarmRecords.front();
        if (rec.status == AlarmStatus::FifthSnooze) {
            snoozeVBox->setVisible(false);
            bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
        }
        timeLabel->setText(TimePointToLocalizedTimeString(presenter->getTimeToDisplay(rec), "%I:%0M"));
        presenter->handleMusicPlay(rec.path);
    }

    bool AlarmReminderWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.is(gui::KeyCode::KEY_ENTER) && alarmRecords.front().status != AlarmStatus::FifthSnooze) {
            presenter->update(alarmRecords.front(), UserAction::Snooze, presenter->getElapsedMinutes());
            closeReminder();
            return true;
        }

        if (inputEvent.is(gui::KeyCode::KEY_RF)) {
            presenter->update(alarmRecords.front(), UserAction::TurnOff, 0);
            closeReminder();
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

    void AlarmReminderWindow::closeReminder()
    {
        if (!alarmRecords.empty()) {
            alarmRecords.erase(alarmRecords.begin());
        }
        if (alarmRecords.empty()) {
            LOG_DEBUG("Switch to alarm main window");
            presenter->stopTimers();
            presenter->stopMusic();
            // app::manager::Controller::sendAction(application, app::manager::actions::Home);
            application->switchWindow(gui::name::window::main_window);
        }
        else {
            LOG_DEBUG("Next alarm at the same time handle");
            presenter->stopTimers();
            presenter->startTimers(getCallback());
            displayAlarm();
        }
    }

    AlarmsReminderModel::OnTimerCallback AlarmReminderWindow::getCallback()
    {
        auto callback = [&]() {
            for (auto &alarm : alarmRecords) {
                presenter->update(alarm, UserAction::Snooze, presenter->getElapsedMinutes());
            }
            alarmRecords.clear();
            closeReminder();
        };
        return callback;
    }
} // namespace app::alarmClock
