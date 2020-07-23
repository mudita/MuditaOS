#include "EventDetailWindow.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include <gui/widgets/Window.hpp>
#include <gui/widgets/Item.hpp>
#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/BottomBar.hpp>
#include <gui/widgets/TopBar.hpp>
#include <time/time_conversion.hpp>

namespace app
{

    EventDetailWindow::EventDetailWindow(app::Application *app, std::string name)
        : AppWindow(app, style::window::calendar::name::details_window)
    {
        buildInterface();
    }

    void EventDetailWindow::rebuild()
    {
        AppWindow::destroyInterface();
        buildInterface();
    }

    void EventDetailWindow::buildInterface()
    {
        AppWindow::buildInterface();

        topBar->setActive(gui::TopBar::Elements::TIME, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::localize.get(style::strings::common::options));

        setTitle(utils::time::Time().str("%d %B"));

        /// TODO: Items size should be resizable to match text in description
        ///       Navigation in text.
        // should be resizable
        int description_h = 50;
        description       = new gui::Text(this,
                                    style::window::default_left_margin,
                                    style::window::calendar::details::description_y,
                                    style::window::default_body_width,
                                    description_h,
                                    "Football with folks at School");
        description->setTextType(gui::TextType::MULTI_LINE);
        description->setEditMode(gui::EditMode::BROWSE);
        description->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        description->setFont(style::window::font::big);
        description->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));

        eventHeading = new gui::Label(this,
                                      style::window::default_left_margin,
                                      style::window::default_left_margin + style::header::height,
                                      style::window::default_body_width,
                                      style::window::label::small_h);
        eventHeading->setPenFocusWidth(0);
        eventHeading->setPenWidth(0);
        eventHeading->setFont(style::window::font::small);
        eventHeading->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        eventHeading->setText(utils::localize.get("app_calendar_event_detail"));
        eventHeading->setLineMode(true);

        time = new gui::Label(this,
                              style::window::default_left_margin,
                              style::window::calendar::details::time_y,
                              style::window::default_body_width,
                              style::window::label::default_h);
        time->setPenFocusWidth(0);
        time->setPenWidth(0);
        time->setFont(style::window::font::bigbold);
        time->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        time->setText("10:00 - 11:00 AM");

        repeatHeading = new gui::Label(this,
                                       style::window::default_left_margin,
                                       style::window::calendar::details::repeat_heading_y,
                                       style::window_width / 2 - 2 * style::window::default_left_margin,
                                       style::window::label::small_h);
        repeatHeading->setPenFocusWidth(0);
        repeatHeading->setPenWidth(0);
        repeatHeading->setFont(style::window::font::small);
        repeatHeading->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        repeatHeading->setText(utils::localize.get("app_calendar_event_detail_repeat"));
        repeatHeading->setLineMode(true);

        repeat = new gui::Label(this,
                                style::window::default_left_margin,
                                style::window::calendar::details::repeat_y,
                                style::window_width / 2 - 2 * style::window::default_left_margin,
                                style::window::label::small_h);
        repeat->setPenFocusWidth(0);
        repeat->setPenWidth(0);
        repeat->setFont(style::window::font::medium);
        repeat->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        repeat->setText(utils::localize.get("app_calendar_repeat_daily"));

        reminderHeading = new gui::Label(this,
                                         style::window_width / 2,
                                         style::window::calendar::details::remind_heading_y,
                                         style::window_width / 2 - style::window::default_left_margin,
                                         style::window::label::small_h);
        reminderHeading->setPenFocusWidth(0);
        reminderHeading->setPenWidth(0);
        reminderHeading->setFont(style::window::font::small);
        reminderHeading->setAlignment(
            gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        reminderHeading->setText(utils::localize.get("app_calendar_event_detail_reminder"));
        reminderHeading->setLineMode(true);

        reminder = new gui::Label(this,
                                  style::window_width / 2,
                                  style::window::calendar::details::remind_y,
                                  style::window_width / 2 - style::window::default_left_margin,
                                  style::window::label::small_h);
        reminder->setPenFocusWidth(0);
        reminder->setPenWidth(0);
        reminder->setFont(style::window::font::medium);
        reminder->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center});
        reminder->setText(utils::localize.get("app_calendar_reminder_1_week_before"));

        setFocusItem(description);
    }

    bool EventDetailWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_LF) {
            LOG_DEBUG("Switch to option window");
            return true;
        }

        return false;
    }
} /* namespace app */
