// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-bell-settings/ApplicationBellSettings.hpp"
#include "BellSettingsStyle.hpp"
#include "windows/BellSettingsTimeUnitsWindow.hpp"

#include <gui/input/InputEvent.hpp>
#include <apps-common/options/OptionStyle.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <widgets/SideListView.hpp>

namespace gui
{
    BellSettingsTimeUnitsWindow::BellSettingsTimeUnitsWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::bell_settings::TimeUnitsWindowContract::Presenter> &&windowPresenter,
        std::string name)
        : AppWindow(app, name), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();

        finishedCallback = [this]() {
            application->switchWindow(
                window::bell_finished::defaultName,
                BellFinishedWindowData::Factory::create("circle_success_big", gui::name::window::main_window));
        };
    }

    void BellSettingsTimeUnitsWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellSettingsTimeUnitsWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        presenter->createData();

        sidelistview = new SideListView(
            this, 0U, 0U, this->getWidth(), this->getHeight(), presenter->getPagesProvider(), PageBarType::None);
        sidelistview->setEdges(RectangleEdge::None);

        sidelistview->rebuildList(listview::RebuildType::Full);

        presenter->loadData();
    }

    void BellSettingsTimeUnitsWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        setFocusItem(sidelistview);
    }

    bool BellSettingsTimeUnitsWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (sidelistview->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            presenter->saveData();
            if (finishedCallback) {
                finishedCallback();
            }
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            if (returnCallback) {
                returnCallback();
                return true;
            }
        }
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }
} /* namespace gui */
