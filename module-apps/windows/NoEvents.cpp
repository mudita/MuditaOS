// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NoEvents.hpp"
#include <service-appmgr/ApplicationManager.hpp>
#include <i18/i18.hpp>
#include <module-services/service-db/messages/QueryMessage.hpp>
#include <module-db/Interface/EventsRecord.hpp>
#include <module-apps/application-calendar/ApplicationCalendar.hpp>
#include <module-db/queries/calendar/QueryEventsGetFiltered.hpp>

using namespace gui;

namespace style
{
    namespace arrow
    {
        const inline uint32_t x = 30;
        const inline uint32_t y = 62;
    } // namespace arrow

    namespace cross
    {
        const inline uint32_t x = 48;
        const inline uint32_t y = 55;
    } // namespace cross
} // namespace style

NoEvents::NoEvents(app::Application *app, const std::string &name, const NoEvents::Meta &meta)
    : gui::AppWindow(app, name), meta(meta)
{}

void NoEvents::buildInterface()
{
    AppWindow::buildInterface();

    topBar->setActive(gui::TopBar::Elements::TIME, true);
    bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
    bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

    auto app = dynamic_cast<app::ApplicationCalendar *>(application);
    assert(app != nullptr);
    if (app->getEquivalentToEmptyWindow() == EquivalentWindow::AllEventsWindow) {
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::localize.get("app_calendar_bar_month"));
    }

    setTitle(meta.title);
    leftArrowImage = new gui::Image(this, style::arrow::x, style::arrow::y, 0, 0, "arrow_left");
    newEventImage  = new gui::Image(this, style::cross::x, style::cross::y, 0, 0, "cross");

    emptyListIcon = new gui::Icon(this,
                                  0,
                                  style::header::height,
                                  style::window_width,
                                  style::window_height - style::header::height - style::footer::height,
                                  meta.icon,
                                  utils::localize.get(meta.text));
}

void NoEvents::update(const Meta &meta)
{
    this->meta = meta;
    rebuild();
}

void NoEvents::rebuild()
{
    AppWindow::destroyInterface();
    erase();
    buildInterface();
}

bool NoEvents::onInput(const gui::InputEvent &inputEvent)
{
    auto app = dynamic_cast<app::ApplicationCalendar *>(application);
    assert(app != nullptr);

    if (inputEvent.keyCode == gui::KeyCode::KEY_RF && inputEvent.state == gui::InputEvent::State::keyReleasedShort) {
        if (app->getEquivalentToEmptyWindow() == EquivalentWindow::DayEventsWindow) {
            app->returnToPreviousWindow();
        }
        else if (app->getEquivalentToEmptyWindow() == EquivalentWindow::AllEventsWindow) {
            LOG_DEBUG("Switch to desktop");
            sapm::ApplicationManager::messageSwitchPreviousApplication(application);
        }
    }

    if (AppWindow::onInput(inputEvent)) {
        return true;
    }

    if (!inputEvent.isShortPress()) {
        return false;
    }

    if (inputEvent.keyCode == gui::KeyCode::KEY_LEFT) {
        return meta.action();
    }

    if (inputEvent.keyCode == gui::KeyCode::KEY_LF &&
        app->getEquivalentToEmptyWindow() == EquivalentWindow::AllEventsWindow) {
        application->switchWindow(gui::name::window::main_window);
        LOG_DEBUG("Switch to month view - main window");
        return true;
    }

    return false;
}