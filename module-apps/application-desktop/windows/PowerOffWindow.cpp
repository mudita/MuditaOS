/*
 * @file PowerOff.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 4 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "log/log.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"

// module-utils
#include "i18/i18.hpp"

#include "PowerOffWindow.hpp"
#include "../ApplicationDesktop.hpp"

// services
#include "service-appmgr/ApplicationManager.hpp"

#include "service-cellular/ServiceCellular.hpp"
#include <Style.hpp>

namespace gui
{

    PowerOffWindow::PowerOffWindow(app::Application *app) : AppWindow(app, app::window::name::desktop_poweroff)
    {
        buildInterface();
    }

    void PowerOffWindow::rebuild()
    {
        // find which widget has focus
        uint32_t index = 0;
        for (uint32_t i = 0; i < selectionLabels.size(); i++)
            if (selectionLabels[i] == getFocusItem()) {
                index = i;
                break;
            }

        destroyInterface();
        buildInterface();
        setFocusItem(selectionLabels[index]);
    }
    void PowerOffWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::confirm));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        powerImage     = new gui::Image(this, 177, 132, 0, 0, "pin_lock_info");
        powerDownImage = new gui::Image(this, 0, 0, 0, 0, "logo");
        powerDownImage->setVisible(false);

        // title label
        titleLabel = new gui::Label(this, 0, 60, 480, 40);
        titleLabel->setFilled(false);
        titleLabel->setBorderColor(gui::ColorFullBlack);
        titleLabel->setFont(style::header::font::title);
        titleLabel->setText(utils::localize.get("app_desktop_poweroff_title"));
        titleLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        titleLabel->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

        // label with question for powering down
        infoLabel = new gui::Label(this, 0, 294, 480, 30);
        infoLabel->setFilled(false);
        infoLabel->setBorderColor(gui::ColorNoColor);
        infoLabel->setFont(style::window::font::medium);
        infoLabel->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
        infoLabel->setText(utils::localize.get("app_desktop_poweroff_question"));

        uint32_t pinLabelX = 46;
        // TODO change this to 397 after power manager is finished
        uint32_t pinLabelY = 350;
        for (uint32_t i = 0; i < 4; i++) {
            gui::Label *label = new gui::Label(this, pinLabelX, pinLabelY, 193, 75);
            label->setFilled(false);
            label->setBorderColor(gui::ColorFullBlack);
            label->setPenWidth(0);
            label->setPenFocusWidth(2);
            label->setRadius(5);
            label->setFont(style::window::font::medium);
            label->setEdges(RectangleEdgeFlags::GUI_RECT_ALL_EDGES);
            label->setAlignment(
                gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
            selectionLabels.push_back(label);
            pinLabelX += 193;
        }

        selectionLabels[0]->setText(utils::localize.get(style::strings::common::no));
        selectionLabels[1]->setText(utils::localize.get(style::strings::common::yes));

        pinLabelX = 46;
        pinLabelY += 75;
        eventMgrLabel = new gui::Label(this, pinLabelX, pinLabelY, 193 * 2, 75);
        eventMgrLabel->setFilled(false);
        eventMgrLabel->setBorderColor(gui::ColorFullBlack);
        eventMgrLabel->setPenWidth(0);
        eventMgrLabel->setPenFocusWidth(2);
        eventMgrLabel->setRadius(5);
        eventMgrLabel->setFont(style::window::font::bigbold);
        eventMgrLabel->setText("TURN PWR MGR OFF");
        eventMgrLabel->setEdges(RectangleEdgeFlags::GUI_RECT_ALL_EDGES);
        eventMgrLabel->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));

        // define navigation between labels
        selectionLabels[0]->setNavigationItem(NavigationDirection::LEFT, selectionLabels[1]);
        selectionLabels[0]->setNavigationItem(NavigationDirection::RIGHT, selectionLabels[1]);
        selectionLabels[0]->setNavigationItem(NavigationDirection::DOWN, eventMgrLabel);

        selectionLabels[1]->setNavigationItem(NavigationDirection::LEFT, selectionLabels[0]);
        selectionLabels[1]->setNavigationItem(NavigationDirection::RIGHT, selectionLabels[0]);
        selectionLabels[1]->setNavigationItem(NavigationDirection::DOWN, eventMgrLabel);

        eventMgrLabel->setNavigationItem(NavigationDirection::UP, selectionLabels[0]);

        // callbacks for getting focus
        selectionLabels[0]->focusChangedCallback = [=](gui::Item &item) {
            if (item.focus)
                this->state = State::Return;
            return true;
        };

        selectionLabels[1]->focusChangedCallback = [=](gui::Item &item) {
            if (item.focus)
                this->state = State::PowerDown;
            return true;
        };

        selectionLabels[1]->activatedCallback = [=](gui::Item &item) {
            LOG_INFO("Closing system");
            application->setShutdownFlag();

            bottomBar->setVisible(false);
            topBar->setVisible(false);
            selectionLabels[0]->setVisible(false);
            selectionLabels[1]->setVisible(false);
            eventMgrLabel->setVisible(false);
            powerImage->setVisible(false);
            powerDownImage->setVisible(true);
            titleLabel->setVisible(false);
            infoLabel->setVisible(false);

            application->refreshWindow(RefreshModes::GUI_REFRESH_DEEP);

            return true;
        };

        // TODO Mati pisze tutaj.
        eventMgrLabel->activatedCallback = [=](gui::Item &item) {
            static bool state = false;
            if (state == false) {
                // sys::SystemManager::DestroyService(ServiceCellular::serviceName,application);
                sys::SystemManager::SuspendSystem(application);
                LOG_INFO("SUSPEND SYSTEM");
                state = true;
            }
            else {
                sys::SystemManager::ResumeSystem(application);
                LOG_INFO("RESUME SYSTEM");
                state = false;
            }
            return true;
        };
    }
    void PowerOffWindow::destroyInterface()
    {
        AppWindow::destroyInterface();
        if (titleLabel) {
            removeWidget(titleLabel);
            delete titleLabel;
            titleLabel = nullptr;
        };
        if (infoLabel) {
            removeWidget(infoLabel);
            delete infoLabel;
            infoLabel = nullptr;
        };

        for (uint32_t i = 0; i < selectionLabels.size(); i++) {
            if (selectionLabels[i]) {
                removeWidget(selectionLabels[i]);
                delete selectionLabels[i];
                selectionLabels[i] = nullptr;
            };
        }
        selectionLabels.clear();

        if (powerImage) {
            removeWidget(powerImage);
            delete powerImage;
            powerImage = nullptr;
        };
        if (powerDownImage) {
            removeWidget(powerDownImage);
            delete powerDownImage;
            powerDownImage = nullptr;
        };

        focusItem = nullptr;
        delete eventMgrLabel;
        children.clear();
    }

    PowerOffWindow::~PowerOffWindow()
    {
        destroyInterface();
    }

    void PowerOffWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        // on entering screen always set default result as returning to home screen and set focus to "No" label
        state = State::Return;
        setFocusItem(selectionLabels[0]);
    }

    bool PowerOffWindow::onInput(const InputEvent &inputEvent)
    {
        // check if any of the lower inheritance onInput methods catch the event
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        // process only short press, consume rest
        if (inputEvent.state != gui::InputEvent::State::keyReleasedShort)
            return true;

        // if enter was pressed check state and power down or return to main desktop's window
        if (inputEvent.keyCode == KeyCode::KEY_ENTER) {
            if (state == State::PowerDown) {
                // TODO start powering down procedure
            }
            else {
                application->switchWindow("MainWindow");
            }
        }

        return false;
    }

} /* namespace gui */
