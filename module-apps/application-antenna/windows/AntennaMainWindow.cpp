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
#include <Style.hpp>

#include "../ApplicationAntenna.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"

namespace gui
{

    AntennaMainWindow::AntennaMainWindow(app::Application *app) : AppWindow(app, gui::name::window::main_window)
    {
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
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_open"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

        topBar->setActive(TopBar::Elements::TIME, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);

        setTitle(utils::localize.get("app_desktop_tools_antenna"));

        for (auto title : titlesText) {
            titles.push_back(addLabel(title, [=](gui::Item &item) { return true; }));
        }
        titles[3]->setPenWidth(0);

        uint32_t posY = 110;
        for (uint32_t i = 0; i < titles.size(); i++) {
            titles[i]->setPosition(40, posY);
            posY += 33 + 5;
        }

        buttons.push_back(addLabel("Antenna A", [=](gui::Item &) {
            buttons[0]->setFont(style::window::font::mediumbold);
            buttons[1]->setFont(style::window::font::medium);
            CellularServiceAPI::SelectAntenna(application, 0);
            return true;
        }));
        buttons.push_back(addLabel("Antenna B", [=](gui::Item &) {
            buttons[0]->setFont(style::window::font::medium);
            buttons[1]->setFont(style::window::font::mediumbold);
            CellularServiceAPI::SelectAntenna(application, 1);
            return true;
        }));

        buttons.push_back(addLabel("Start operators scan", [=](gui::Item &) {
            buttons[2]->setText("Scan in progress");
            CellularServiceAPI::StartOperatorsScan(this->application);
            this->application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            auto app = dynamic_cast<app::ApplicationAntenna *>(this->application);
            if (app != nullptr) {
                app->setScanInProgress(true);
            }
            return true;
        }));
        uint32_t posX = 40;
        for (uint32_t i = 0; i < buttons.size(); i++) {
            buttons[i]->setFont(style::window::font::medium);
            buttons[i]->setPosition(posX, 500);
            buttons[i]->setAlignement(
                gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
            posX += 240;
        }

        buttons[0]->setPosition(40, 517);
        buttons[0]->setSize(200, 33);

        buttons[1]->setPosition(240, 517);
        buttons[1]->setSize(200, 33);

        buttons[2]->setPosition(40, 479);
        buttons[2]->setSize(400, 33);

        operators = new gui::Text(this, 40, 260, 400, 214);
        operators->setRadius(11);
        operators->setFont(style::window::font::verysmall);

        buttons[2]->setNavigationItem(NavigationDirection::DOWN, buttons[0]);
        buttons[2]->setNavigationItem(NavigationDirection::UP, operators);

        operators->setNavigationItem(NavigationDirection::DOWN, buttons[2]);

        buttons[0]->setNavigationItem(NavigationDirection::RIGHT, buttons[1]);
        buttons[0]->setNavigationItem(NavigationDirection::LEFT, buttons[1]);
        buttons[0]->setNavigationItem(NavigationDirection::UP, buttons[2]);
        buttons[1]->setNavigationItem(NavigationDirection::RIGHT, buttons[0]);
        buttons[1]->setNavigationItem(NavigationDirection::LEFT, buttons[0]);
        buttons[1]->setNavigationItem(NavigationDirection::UP, buttons[2]);

        setFocusItem(buttons[0]);
        buttons[0]->setFont(style::window::font::mediumbold);
        CellularServiceAPI::SelectAntenna(application, 0);
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
    {}

    bool AntennaMainWindow::onInput(const InputEvent &inputEvent)
    {
        bool ret = AppWindow::onInput(inputEvent);
        if (ret) {
            application->render(RefreshModes::GUI_REFRESH_FAST);
            return true;
        }
        return false;
    }

    gui::Label *AntennaMainWindow::addLabel(const UTF8 &text,
                                            std::function<bool(Item &)> activatedCallback,
                                            bool visibleBorder)
    {
        gui::Label *label = new gui::Label(this, 40, 500, 400, 33, text);
        label->setMargins(gui::Margins(16, 0, 0, 0));
        label->setFilled(false);
        label->setPenFocusWidth(3);
        if (visibleBorder) {
            label->setPenWidth(1);
        }
        else {
            label->setPenWidth(0);
        }
        label->setFont(style::window::font::verysmall);
        label->setAlignement(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER));
        label->setRadius(11);
        label->activatedCallback = activatedCallback;

        return label;
    }

    void AntennaMainWindow::updateDebugInfo(std::vector<std::string> &data)
    {
        LOG_INFO("AntennaMainWindow::updateDebugInfo vector size %d", data.size());
        if (data.size() == 3) {
            titles[static_cast<uint32_t>(labelDescripion::csq)]->setText(
                titlesText[static_cast<uint32_t>(labelDescripion::csq)] + data[0]);
            titles[static_cast<uint32_t>(labelDescripion::status)]->setText(
                titlesText[static_cast<uint32_t>(labelDescripion::status)] + data[1]);
            titles[static_cast<uint32_t>(labelDescripion::band)]->setText(
                titlesText[static_cast<uint32_t>(labelDescripion::band)] + data[2]);
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
        }
    }
    void AntennaMainWindow::updateOperatorsScan(std::vector<std::string> &data)
    {
        buttons[2]->setText("Start operators scan");
        std::string formattedString;
        for (auto element : data) {
            formattedString += element + "\n";
        }
        std::replace(formattedString.begin(), formattedString.end(), '\"', ' ');
        operators->setText(formattedString);
        this->application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }
} // namespace gui
