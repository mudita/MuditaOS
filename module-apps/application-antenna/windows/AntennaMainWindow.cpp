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

#include "../AntennaAppStyle.hpp"
#include "ScanModesWindow.hpp"
#include "../ApplicationAntenna.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"
#include "service-antenna/api/AntennaServiceAPI.hpp"
#include <at/response.hpp>
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
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::TIME, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);

        setTitle(utils::localize.get("app_desktop_tools_antenna"));

        for (auto title : titlesText) {
            titles.push_back(addLabel(title, [=](gui::Item &item) { return true; }));
        }
        titles[3]->setPenWidth(0);

        uint32_t posY = 110;
        for (uint32_t i = 0; i < titles.size(); i++) {
            titles[i]->setPosition(antenna::main_window::commonXPos, posY);
            posY += antenna::main_window::commonYOffset;
        }

        buttons.push_back(addLabel("Antenna A", [=](gui::Item &) {
            CellularServiceAPI::SelectAntenna(application, bsp::cellular::antenna::lowBand);
            return true;
        }));

        buttons.push_back(addLabel("Antenna B", [=](gui::Item &) {
            CellularServiceAPI::SelectAntenna(application, bsp::cellular::antenna::highBand);
            return true;
        }));

        buttons.push_back(addLabel("Start operators scan", [=](gui::Item &) {
            buttons[buttonDescriotion::StartScan]->setText("Scan in progress");

            this->application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            auto app = dynamic_cast<app::ApplicationAntenna *>(this->application);
            if (app != nullptr) {
                app->setScanInProgress(true);
            }
            CellularServiceAPI::StartOperatorsScan(this->application);
            return true;
        }));

        buttons.push_back(addLabel("Scan mode", [=](gui::Item &) {
            this->application->switchWindow(gui::name::window::scan_window, nullptr);
            return true;
        }));

        buttons.push_back(addLabel("Lock antenna", [=](gui::Item &) {
            antenna::lockState currentState;
            if (AntennaServiceAPI::GetLockState(this->application, currentState)) {
                auto newState = currentState == antenna::lockState::locked ? antenna::lockState::unlocked
                                                                           : antenna::lockState::locked;
                if (AntennaServiceAPI::LockRequest(this->application, newState)) {
                    if (AntennaServiceAPI::GetLockState(this->application, currentState)) {
                        updateLockedButton(currentState);
                    }
                }
            }
            return true;
        }));

        uint32_t posX = antenna::main_window::commonXPos;
        for (uint32_t i = 0; i < buttons.size(); i++) {
            buttons[i]->setFont(style::window::font::medium);
            buttons[i]->setPosition(posX, 500);
            buttons[i]->setAlignment(
                gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
            posX += 240;
        }

        buttons[buttonDescriotion::AntennaA]->setPosition(antenna::main_window::buttonPosXLeft,
                                                          antenna::main_window::buttonPosYRow2);
        buttons[buttonDescriotion::AntennaA]->setSize(antenna::main_window::buttonSmallW,
                                                      antenna::main_window::buttonH);

        buttons[buttonDescriotion::AntennaB]->setPosition(antenna::main_window::buttonPosXCenter,
                                                          antenna::main_window::buttonPosYRow2);
        buttons[buttonDescriotion::AntennaB]->setSize(antenna::main_window::buttonSmallW,
                                                      antenna::main_window::buttonH);

        buttons[buttonDescriotion::StartScan]->setPosition(antenna::main_window::commonXPos,
                                                           antenna::main_window::buttonPosYRow1);
        buttons[buttonDescriotion::StartScan]->setSize(antenna::main_window::buttonBigW, antenna::main_window::buttonH);

        buttons[buttonDescriotion::ScanMode]->setPosition(antenna::main_window::buttonPostXRight,
                                                          antenna::main_window::buttonPosYRow2);
        buttons[buttonDescriotion::ScanMode]->setSize(antenna::main_window::buttonSmallW,
                                                      antenna::main_window::buttonH);

        buttons[buttonDescriotion::LockAntennaManager]->setPosition(antenna::main_window::buttonPosXLeft,
                                                                    antenna::main_window::buttonPosYRow3);
        buttons[buttonDescriotion::LockAntennaManager]->setSize(antenna::main_window::buttonBigW,
                                                                antenna::main_window::buttonH);

        operators = new gui::Text(this,
                                  antenna::main_window::commonXPos,
                                  antenna::main_window::scanListPosY,
                                  antenna::main_window::scanListW,
                                  antenna::main_window::scanListH);
        operators->setRadius(11);
        operators->setFont(style::window::font::verysmall);

        buttons[buttonDescriotion::StartScan]->setNavigationItem(NavigationDirection::DOWN,
                                                                 buttons[buttonDescriotion::AntennaA]);
        buttons[buttonDescriotion::StartScan]->setNavigationItem(NavigationDirection::UP, operators);

        operators->setNavigationItem(NavigationDirection::DOWN, buttons[buttonDescriotion::StartScan]);

        buttons[buttonDescriotion::AntennaA]->setNavigationItem(NavigationDirection::RIGHT,
                                                                buttons[buttonDescriotion::AntennaB]);
        buttons[buttonDescriotion::AntennaA]->setNavigationItem(NavigationDirection::LEFT,
                                                                buttons[buttonDescriotion::ScanMode]);
        buttons[buttonDescriotion::AntennaA]->setNavigationItem(NavigationDirection::UP,
                                                                buttons[buttonDescriotion::StartScan]);
        buttons[buttonDescriotion::AntennaA]->setNavigationItem(NavigationDirection::DOWN,
                                                                buttons[buttonDescriotion::LockAntennaManager]);

        buttons[buttonDescriotion::AntennaB]->setNavigationItem(NavigationDirection::RIGHT,
                                                                buttons[buttonDescriotion::ScanMode]);
        buttons[buttonDescriotion::AntennaB]->setNavigationItem(NavigationDirection::LEFT,
                                                                buttons[buttonDescriotion::AntennaA]);
        buttons[buttonDescriotion::AntennaB]->setNavigationItem(NavigationDirection::UP,
                                                                buttons[buttonDescriotion::StartScan]);
        buttons[buttonDescriotion::AntennaB]->setNavigationItem(NavigationDirection::DOWN,
                                                                buttons[buttonDescriotion::LockAntennaManager]);

        buttons[buttonDescriotion::ScanMode]->setNavigationItem(NavigationDirection::RIGHT,
                                                                buttons[buttonDescriotion::AntennaA]);
        buttons[buttonDescriotion::ScanMode]->setNavigationItem(NavigationDirection::LEFT,
                                                                buttons[buttonDescriotion::AntennaB]);
        buttons[buttonDescriotion::ScanMode]->setNavigationItem(NavigationDirection::UP,
                                                                buttons[buttonDescriotion::StartScan]);
        buttons[buttonDescriotion::ScanMode]->setNavigationItem(NavigationDirection::DOWN,
                                                                buttons[buttonDescriotion::LockAntennaManager]);

        buttons[buttonDescriotion::LockAntennaManager]->setNavigationItem(NavigationDirection::UP,
                                                                          buttons[buttonDescriotion::AntennaA]);
        setFocusItem(buttons[buttonDescriotion::AntennaA]);

        antenna::lockState antennaServiceState;
        if (AntennaServiceAPI::GetLockState(this->application, antennaServiceState)) {
            updateLockedButton(antennaServiceState);
        }

        bsp::cellular::antenna antenna;
        if (CellularServiceAPI::GetAntenna(this->application, antenna)) {
            updateAntennaButtons(antenna);
        }
    }
    void AntennaMainWindow::destroyInterface()
    {
        erase();
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
        gui::Label *label = new gui::Label(this,
                                           antenna::main_window::commonXPos,
                                           0,
                                           antenna::main_window::titleW,
                                           antenna::main_window::titleH,
                                           text);
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
        label->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER));
        label->setRadius(11);
        label->activatedCallback = activatedCallback;

        return label;
    }

    void AntennaMainWindow::updateDebugInfo(std::vector<std::string> &data)
    {
        LOG_INFO("AntennaMainWindow::updateDebugInfo vector size %u", static_cast<unsigned int>(data.size()));
        if (data.size() == 3) {
            titles[static_cast<uint32_t>(labelDescripion::csq)]->setText(
                titlesText[static_cast<uint32_t>(labelDescripion::csq)] + data[0]);
            titles[static_cast<uint32_t>(labelDescripion::status)]->setText(
                titlesText[static_cast<uint32_t>(labelDescripion::status)] + data[1]);

            auto bandFrequency = at::response::qnwinfo::parseNetworkFrequency(data[2]);
            titles[static_cast<uint32_t>(labelDescripion::band)]->setText(
                titlesText[static_cast<uint32_t>(labelDescripion::band)] + data[2] + "   " +
                std::to_string(bandFrequency));
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
        }
    }
    void AntennaMainWindow::updateOperatorsScan(std::vector<std::string> &data)
    {
        buttons[buttonDescriotion::StartScan]->setText("Start operators scan");
        std::string formattedString;
        for (auto element : data) {
            formattedString += element + "\n";
        }
        std::replace(formattedString.begin(), formattedString.end(), '\"', ' ');
        operators->setText(formattedString);
        this->application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }
    void AntennaMainWindow::updateAntennaButtons(bsp::cellular::antenna antenna)
    {
        bool updated = false;
        if (antenna == bsp::cellular::antenna::lowBand) {
            buttons[buttonDescriotion::AntennaA]->setFont(style::window::font::mediumbold);
            buttons[buttonDescriotion::AntennaB]->setFont(style::window::font::medium);
            updated = true;
        }
        else if (antenna == bsp::cellular::antenna::highBand) {
            buttons[buttonDescriotion::AntennaA]->setFont(style::window::font::medium);
            buttons[buttonDescriotion::AntennaB]->setFont(style::window::font::mediumbold);
            updated = true;
        }
        if (updated) {
            application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
        }
    }

    void AntennaMainWindow::updateLockedButton(antenna::lockState antennaState)
    {
        UTF8 buttonText;
        if (antennaState == antenna::lockState::locked) {
            buttonText = "Unlock antenna.";
        }
        else if (antennaState == antenna::lockState::unlocked) {
            buttonText = "Lock antenna.";
        }

        buttons[buttonDescriotion::LockAntennaManager]->setText(buttonText);
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    }
} // namespace gui
