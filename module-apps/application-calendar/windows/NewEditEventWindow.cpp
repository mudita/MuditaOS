#include "NewEditEventWindow.hpp"
#include "application-calendar/widgets/CheckBoxItem.hpp"
#include "application-calendar/widgets/EventTimeItem.hpp"
#include <gui/widgets/Window.hpp>
#include <gui/widgets/Span.hpp>
#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/BottomBar.hpp>
#include <gui/widgets/TopBar.hpp>

#include <time/time_conversion.hpp>

namespace app
{

    NewEditEventWindow::NewEditEventWindow(app::Application *app, std::string name)
        : AppWindow(app, style::window::calendar::name::new_edit_event)
    {
        buildInterface();
    }

    void NewEditEventWindow::rebuild()
    {
        buildInterface();
    }

    void NewEditEventWindow::buildInterface()
    {
        AppWindow::buildInterface();

        auto ttime = utils::time::Time();
        topBar->setActive(gui::TopBar::Elements::TIME, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::save));

        setTitle("New/Edit event");

        // Event name box
        eventNameVBox = new gui::VBox(this, 15, 120, 400, 230); // 70
        eventNameVBox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        eventNameLabel = new gui::Label(eventNameVBox, 0, 0, 400, 20);
        eventNameLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        eventNameLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        eventNameLabel->setFont(style::window::font::small);
        eventNameLabel->setText("Event name");
        eventNameLabel->activeItem = false;

        // new gui::Span(eventNameVBox, gui::Axis::Y, 30); // spread title & eventNameInput

        eventNameInput = new gui::Text(eventNameVBox, 0, 0, 400, 50);
        eventNameInput->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        eventNameInput->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        eventNameInput->setFont(style::window::font::medium);
        eventNameInput->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &text) { bottomBarTemporaryMode(text); },
            [=]() { bottomBarRestoreFromTemporaryMode(); },
            [=]() { selectSpecialCharacter(); }));
        eventNameInput->setPenFocusWidth(style::window::default_border_focus_w);
        eventNameInput->setPenWidth(1);
        eventNameInput->setEditMode(gui::EditMode::EDIT);

        // All day event box
        checkBox = new gui::CheckBoxItem("All day event");
        eventNameVBox->addWidget(checkBox);

        // time
        startTime = new gui::EventTimeItem("Start");
        eventNameVBox->addWidget(startTime);
        // setFocusItem(eventNameInput);
        setFocusItem(eventNameVBox);
    }

    bool NewEditEventWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_ENTER) {
            // LOG_DEBUG("TODO: Save event");
            LOG_DEBUG("TEMPORARY: Switch to custom repeat window");
            application->switchWindow(style::window::calendar::name::custom_repeat_window);
            return true;
        }

        return false;
    }
} /* namespace app */
