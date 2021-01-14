// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventReminderWindow.hpp"
#include "application-popup/ApplicationPopup.hpp"
#include "module-apps/application-calendar/data/CalendarData.hpp"
#include <gui/widgets/Window.hpp>
#include <time/time_conversion.hpp>
#include "service-appmgr/Controller.hpp"
#include <module-db/queries/notifications/QueryNotificationsClear.hpp>
#include <module-db/queries/notifications/QueryNotificationsIncrement.hpp>

namespace gui
{
    constexpr static const int reminderLifeDuration = 10000;

    EventReminderWindow::EventReminderWindow(app::Application *app, std::string name)
        : AppWindow(app, style::window::name::event_reminder_window)
    {

        buildInterface();

        reminderTimer = std::make_unique<sys::Timer>(
            "CalendarReminderTimer", app, reminderLifeDuration, sys::Timer::Type::SingleShot);
        reminderTimer->connect([=](sys::Timer &) { reminderTimerCallback(); });
    }

    EventReminderWindow::~EventReminderWindow()
    {
        destroyTimer();
    }

    void EventReminderWindow::onClose()
    {
        destroyTimer();
        if (!eventRecords->empty()) {
            rebuild();
        }
    }

    void EventReminderWindow::rebuild()
    {
        erase();
        buildInterface();
        provideEventData();
        startTimer();
    }

    top_bar::Configuration EventReminderWindow::configureTopBar(top_bar::Configuration appConfiguration)
    {
        using namespace top_bar;
        appConfiguration.enable({Indicator::Signal,
                                 Indicator::Time,
                                 Indicator::Battery,
                                 Indicator::SimCard,
                                 Indicator::NetworkAccessTechnology});
        return appConfiguration;
    }

    void EventReminderWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::ok));
        bottomBar->setBorderColor(ColorNoColor);

        const uint32_t w = this->getWidth() - style::window::default_left_margin - style::window::default_right_margin;
        const uint32_t h = this->getHeight() - bottomBar->getHeight();
        body             = new gui::VBox(this, style::window::default_left_margin, topBar->getHeight(), w, h);
        body->setBorderColor(gui::ColorNoColor);

        topImage = new gui::Image(body,
                                  style::window::calendar::imageCircleTop::x,
                                  style::window::calendar::imageCircleTop::y,
                                  0,
                                  0,
                                  style::window::calendar::imageCircleTop::name);
        topImage->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center));

        dateLabel = new Label(body, 0, 0, w, style::window::label::default_h);
        dateLabel->setFont(style::window::font::smallbold);
        dateLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        dateLabel->setBorderColor(ColorNoColor);

        timeLabel = new Label(body, 0, 0, w, style::window::label::default_h);
        timeLabel->setFont(style::window::font::largelight);
        timeLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        timeLabel->setBorderColor(ColorNoColor);

        bottomImage = new gui::Image(body,
                                     style::window::calendar::imageCircleBottom::x,
                                     style::window::calendar::imageCircleBottom::y,
                                     0,
                                     0,
                                     style::window::calendar::imageCircleBottom::name);
        bottomImage->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center));

        descriptionLabel = new Label(body, 0, 0, w, 2 * style::window::label::default_h);
        descriptionLabel->setFont(style::window::font::big);
        descriptionLabel->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        descriptionLabel->setBorderColor(ColorNoColor);

        setFocusItem(body);
    }

    void EventReminderWindow::provideEventData()
    {
        if (!eventRecords->empty()) {
            auto eventRecord = eventRecords->back();
            eventRecords->pop_back();
            dateLabel->setText(TimePointToLocalizedDateString(eventRecord.date_from));
            timeLabel->setText(TimePointToLocalizedTimeString(eventRecord.date_from));
            descriptionLabel->setText(eventRecord.title);
        }
        else {
            LOG_ERROR("Events vector is empty!");
        }
    }

    void EventReminderWindow::destroyInterface()
    {
        destroyTimer();
        erase();
    }

    void EventReminderWindow::startTimer()
    {
        reminderTimer->connect([=](sys::Timer &) { reminderTimerCallback(); });
        reminderTimer->reload();
    }

    void EventReminderWindow::destroyTimer()
    {
        reminderTimer->stop();
    }

    auto EventReminderWindow::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            return false;
        }

        auto *item = dynamic_cast<EventRecordsData *>(data);
        if (item == nullptr) {
            return false;
        }

        eventRecords = item->getData();
        provideEventData();
        startTimer();

        return true;
    }

    bool EventReminderWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_ENTER) {
            closeReminder();
            return true;
        }

        return false;
    }

    void EventReminderWindow::reminderTimerCallback()
    {

        DBServiceAPI::GetQuery(
            application,
            db::Interface::Name::Notifications,
            std::make_unique<db::query::notifications::Increment>(NotificationsRecord::Key::CalendarEvents));

        closeReminder();
    }

    void EventReminderWindow::closeReminder()
    {
        LOG_DEBUG("Switch to previous window");
        destroyTimer();
        if (!eventRecords->empty()) {
            rebuild();
            return;
        }
        app::manager::Controller::sendAction(application, app::manager::actions::ClosePopup);
    }

} /* namespace gui */
