#include "gui/widgets/Window.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "DayWindow.hpp"
#include "../models/CalendarModel.hpp"
#include "../models/CalendarModel.cpp"

#include <time/time_conversion.hpp>
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

namespace app
{

    DayWindow::DayWindow(app::Application *app, std::string name)
        : AppWindow(app, "DayWindow"), model{new CalendarModel(app)}
    {
        buildInterface();
    }

    void DayWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void DayWindow::buildInterface()
    {
        AppWindow::buildInterface();
        emptyDay();

        auto ttime = utils::time::Time();
        topBar->setActive(gui::TopBar::Elements::TIME, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));

        /// TODO: Set title from the exact choosen day
        setTitle(ttime.str("%d %B"));
        leftArrowImage   = new gui::Image(this, 30, 62, 0, 0, "arrow_left");
        newDayEventImage = new gui::Image(this, 48, 55, 0, 0, "cross");

        list = new gui::ListView(this, 16, 110, 480 - 32, 440);

        list->setPenFocusWidth(0);
        list->setPenWidth(0);
        list->setProvider(model);
        // list->setItemSpanSize(style::listview::item_span_small);
        list->setVisible(true);
    
        list->focusChangedCallback = [=](gui::Item & /*item*/) {
            bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
            emptyListText->setVisible(false);
            return true;
        };
        setFocusItem(list);

        emptyListImage->setVisible(false);
        emptyListImage->focusChangedCallback = [=](gui::Item & /*item*/) {
            bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
            emptyListText->setVisible(true);
            return true;
        };
    }

    // prepare assets for empty day window
    void DayWindow::emptyDay()
    {
        emptyListImage = new gui::Image(this,
                                       176,
                                       style::window_height / 2 - 95,
                                       "phonebook_empty_grey_circle_W_G");

        /// TODO: Load text from language assets
        emptyListText = new gui::Text(
            this, 40, style::window_height / 2 + 70, 400, 66, "No events yet.\nPress left arrow to add new.");
        emptyListText->setTextType(gui::Text::TextType::MULTI_LINE);
        emptyListText->setEditMode(gui::Text::EditMode::BROWSE);
        emptyListText->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        emptyListText->setFont(style::window::font::medium);
        emptyListText->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
        emptyListText->setTextColor({7, 0});
    }

    void DayWindow::destroyInterface()
    {
        AppWindow::destroyInterface();
        delete model;
    }

    void DayWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        if (model->getItemCount() == 0) {
            emptyListImage->setVisible(true);
            list->setVisible(false);
            setFocusItem(emptyListImage);
        }
    }

    bool DayWindow::onInput(const gui::InputEvent &inputEvent)
    {
        // process only if key is released
        if ((inputEvent.state != gui::InputEvent::State::keyReleasedShort) ||
            (inputEvent.state != gui::InputEvent::State::keyReleasedLong)) {
            if (inputEvent.keyCode == gui::KeyCode::KEY_LEFT) {
                /// TODO: Switch to new/edit event window
                // application->switchWindow("");
                LOG_DEBUG("New window - edit window");
                return true;
            }
        }

        return AppWindow::onInput(inputEvent);
    }
} /* namespace app */
