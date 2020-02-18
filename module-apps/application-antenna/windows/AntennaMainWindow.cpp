/*
 * AntennaMainWindow.cpp
 *
 *  Created on: 18 lut 2020
 *      Author: kuba
 */
#include "AntennaMainWindow.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"

#include "i18/i18.hpp"
namespace gui
{

    AntennaMainWindow::AntennaMainWindow(app::Application *app) : AppWindow(app, gui::name::window::main_window)
    {
        setSize(style::window_width, style::window_height);
        buildInterface();
    }

    void AntennaMainWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }
    void AntennaMainWindow::buildInterface()
    {

        AppWindow::buildInterface();

        bottomBar->setActive(BottomBar::Side::LEFT, true);

        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("common_options"));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_open"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

        topBar->setActive(TopBar::Elements::TIME, true);

        setTitle(utils::localize.get("app_messages_title_main"));
        //	setFocusItem( list );
    }
    void AntennaMainWindow::destroyInterface()
    {
        AppWindow::destroyInterface();

        children.clear();
    }

    AntennaMainWindow::~AntennaMainWindow()
    {
        destroyInterface();
    }

    void AntennaMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
    }

    bool AntennaMainWindow::onInput(const InputEvent &inputEvent)
    {
        // check if any of the lower inheritance onInput methods catch the event
        if (AppWindow::onInput(inputEvent))
        {
            return true;
        }

        return false;
    }
} // namespace gui
