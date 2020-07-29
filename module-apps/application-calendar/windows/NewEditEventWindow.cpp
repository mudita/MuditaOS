#include "NewEditEventWindow.hpp"
#include "application-calendar/widgets/CheckBoxItem.hpp"
#include "application-calendar/widgets/EventTimeItem.hpp"
#include "application-calendar/widgets/SeveralOptionsItem.hpp"
#include <gui/widgets/BoxLayout.hpp>

namespace gui
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

        topBar->setActive(gui::TopBar::Elements::TIME, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::save));

        body = new gui::VBox(this, 15, 120, style::window::default_body_width, 430);
        body->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        eventNameLabel = new gui::Label(body, 0, 0, style::window::default_body_width, 20);
        eventNameLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        eventNameLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        eventNameLabel->setFont(style::window::font::small);
        eventNameLabel->setText(utils::localize.get("app_calendar_new_edit_event_name"));
        eventNameLabel->activeItem = false;

        eventNameInput = new gui::Text(body, 0, 0, style::window::default_body_width, 50);
        eventNameInput->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        eventNameInput->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));
        eventNameInput->setFont(style::window::font::medium);
        eventNameInput->setInputMode(new InputMode({InputMode::ABC, InputMode::abc, InputMode::digit}));
        eventNameInput->setPenFocusWidth(style::window::default_border_focus_w);
        eventNameInput->setPenWidth(style::window::default_border_rect_no_focus);
        eventNameInput->setEditMode(gui::EditMode::EDIT);

        // All day event box
        checkBox = new gui::CheckBoxItem(application, utils::localize.get("app_calendar_new_edit_event_allday"));
        body->addWidget(checkBox);

        // time
        startTime = new gui::EventTimeItem(utils::localize.get("app_calendar_new_edit_event_start"), true);
        body->addWidget(startTime);

        // repeat
        reminderOptions = new gui::SeveralOptionsItem(
            application,
            utils::localize.get("app_calendar_event_detail_repeat"),
            [=](const UTF8 &text, gui::BottomBar::Side side, bool emptyOthers) {
                application->getCurrentWindow()->bottomBarTemporaryMode(text, side, emptyOthers);
            },
            [=]() { application->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });
        body->addWidget(reminderOptions);

        setFocusItem(body);
    }

    void NewEditEventWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        switch (eventAction) {
        case EventAction::None:
            break;
        case EventAction::Add:
            setTitle(utils::localize.get("app_calendar_new_event_title"));
            break;
        case EventAction::Edit:
            setTitle(utils::localize.get("app_calendar_edit_event_title"));
            break;
        }
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
            LOG_DEBUG("TODO: Save event");
            return true;
        }

        return false;
    }

    bool NewEditEventWindow::handleSwitchData(gui::SwitchData *data)
    {
        if (data == nullptr) {
            return false;
        }

        if (data->getDescription() == "Edit") {
            eventAction = EventAction::Edit;
        }
        else if (data->getDescription() == "New") {
            eventAction = EventAction::Add;
        }
        return false;
    }
} /* namespace gui */
