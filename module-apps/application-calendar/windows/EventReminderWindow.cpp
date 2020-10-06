#include "EventReminderWindow.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "module-apps/application-calendar/data/CalendarData.hpp"
#include <gui/widgets/Window.hpp>
#include <time/time_conversion.hpp>
#include "service-appmgr/ApplicationManager.hpp"

namespace gui
{
    constexpr static const int reminderLifeDuration = 20000;

    EventReminderWindow::EventReminderWindow(app::Application *app, std::string name)
        : AppWindow(app, style::window::calendar::name::event_reminder_window)
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

        setFocusItem(nullptr);
    }

    void EventReminderWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
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

        auto *item = dynamic_cast<EventRecordData *>(data);
        if (item == nullptr) {
            return false;
        }

        eventRecord    = item->getData();
        dateLabel->setText(TimePointToDateString(eventRecord->date_from));
        timeLabel->setText(TimePointToMinuteTimeString(eventRecord->date_from));
        descriptionLabel->setText(eventRecord->title);

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
        closeReminder();
    }

    void EventReminderWindow::closeReminder()
    {
        LOG_DEBUG("Switch to previous window");
        destroyTimer();

        sapm::ApplicationManager::messageSwitchApplication(
            application, "ApplicationDesktop", gui::name::window::main_window, nullptr);

        // TODO: after rework of application switching we would switch here to the last opened window
        //  of the last active application
        // application sapm::ApplicationManager::messageSwitchPreviousApplication(application);
    }

} /* namespace gui */
