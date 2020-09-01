#include "EventReminderWindow.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "module-apps/application-calendar/data/CalendarData.hpp"
#include <gui/widgets/Window.hpp>
#include <time/time_conversion.hpp>

#include "service-appmgr/ApplicationManager.hpp"

namespace gui
{

    EventReminderWindow::EventReminderWindow(app::Application *app, std::string name)
        : AppWindow(app, style::window::calendar::name::event_reminder_window)
    {
        buildInterface();

        reminderTimer = std::make_unique<sys::Timer>("CalendarReminderTimer", app, 20000, sys::Timer::Type::SingleShot);
        reminderTimer->connect([=](sys::Timer &) { reminderTimerCallback(); });

        // reminderTimer

        // reminderTimer->start();

        /*auto app = dynamic_cast<app::ApplicationCall *>(application);
        assert(app != nullptr);
        auto timer = std::make_unique<app::GuiTimer>(app);
        timer->setInterval(app->getDelayedStopTime());
        timerCallback = [app, this](Item &, Timer &timer) {
          app->stopCallTimer();
          setState(State::IDLE);
          detachTimer(timer);
          sapm::ApplicationManager::messageSwitchPreviousApplication(app);
          return true;
        };
        app->connect(std::move(timer), this);*/
    }

    EventReminderWindow::~EventReminderWindow()
    {
        destroyTimer();
    }

    void EventReminderWindow::onClose()
    {
        destroyTimer();
    }

    void EventReminderWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void EventReminderWindow::buildInterface()
    {
        AppWindow::buildInterface();

        topBar->setActive(TopBar::Elements::BATTERY, true);
        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(gui::TopBar::Elements::TIME, true);

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::ok));
        bottomBar->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        topImage = new gui::Image(this, 116, 59, 0, 0, "circle_top");
        topImage->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
        bottomImage = new gui::Image(this, 106, 240, 0, 0, "circle_bottom");
        bottomImage->setAlignment(Alignment(gui::Alignment::Horizontal::Center));

        dateLabel = new Label(this, 0, 120, 480, 80);
        dateLabel->setFont(style::window::font::smallbold);
        dateLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        dateLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        timeLabel = new Label(this, 0, 170, 480, 80);
        timeLabel->setFont(style::window::font::largelight);
        timeLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        timeLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        descriptionLabel = new Label(this, 0, 350, 480, 80);
        descriptionLabel->setFont(style::window::font::big);
        descriptionLabel->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        descriptionLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        // setFocusItem(descriptionLabel);
        setFocusItem(nullptr);
    }

    void EventReminderWindow::destroyInterface()
    {
        destroyTimer();
        erase();
    }

    void EventReminderWindow::startTimer()
    {
        reminderTimer->reload();

        /*if (timerId > 0)
        {
            destroyTimer();
        }
        timerId = application->CreateTimer(20000, false, "EventReminderWindow_Timer");
        application->ReloadTimer(timerId);*/
    }

    void EventReminderWindow::destroyTimer()
    {
        reminderTimer->stop();

        /*if (timerId == 0)
        {
            return;
        }
        application->stopTimer(timerId);
        application->DeleteTimer(timerId);
        timerId = 0;*/
    }

    auto EventReminderWindow::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            return false;
        }

        auto *item = dynamic_cast<EventRecordData *>(data);
        if (item == nullptr) {
            return false;
        }

        eventRecord    = item->getData();
        prevWindowName = item->getWindowName();

        dateLabel->setText(TimePointToDateString(eventRecord->date_from));
        timeLabel->setText(TimePointToTimeString(eventRecord->date_from));
        descriptionLabel->setText(eventRecord->title);

        // setFocusItem(this);

        // application->setActiveWindow("EventReminderWindow");

        /*auto startDate = TimePointToYearMonthDay(eventRecord->date_from);
        std::string monthStr =
            utils::time::Locale::get_month(utils::time::Locale::Month(unsigned(startDate.month()) - 1));
        setTitle(std::to_string(unsigned(startDate.day())) + " " + monthStr);*/

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
            /*auto rec  = std::make_unique<EventsRecord>(*eventRecord);
            auto data = std::make_unique<EventRecordData>(std::move(rec));
            data->setWindowName(prevWindowName);
            application->switchWindow(style::window::calendar::name::events_options, std::move(data));*/

            ////timeLabel->setText(TimePointToTimeString(TimePointNow()));
            closeReminder();
            return true;
        }
        /*else if (inputEvent.keyCode == gui::KeyCode::KEY_2) {
            closeReminder();
            return true;
        }*/

        return false;
    }

    void EventReminderWindow::reminderTimerCallback()
    {
        closeReminder();
    }

    void EventReminderWindow::closeReminder()
    {
        LOG_DEBUG("Switch to previous window");
        destroyTimer();

        sapm::ApplicationManager::messageSwitchApplication(
            application, "ApplicationDesktop", gui::name::window::main_window, nullptr);

        // TODO: after rework of application switching we would switch here to the last opened window ot the last active
        // application sapm::ApplicationManager::messageSwitchPreviousApplication(application);
    }

} /* namespace gui */
