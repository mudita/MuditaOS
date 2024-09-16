// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "BellSettingsRelaxationWindow.hpp"

#include <module-gui/gui/input/InputEvent.hpp>
#include <widgets/SideListView.hpp>
#include <common/windows/BellFinishedWindow.hpp>

namespace gui
{
    BellSettingsRelaxationWindow::BellSettingsRelaxationWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::bell_settings::RelaxationWindowPresenter> &&windowPresenter,
        std::string name)
        : AppWindow(app, std::move(name)), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void BellSettingsRelaxationWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(true);
        navBar->setVisible(false);

        listView = new SideListView(
            this, 0U, 0U, this->getWidth(), this->getHeight(), presenter->getPagesProvider(), PageBarType::None);
        listView->setEdges(RectangleEdge::None);

        listView->rebuildList(listview::RebuildType::Full);

        presenter->loadData();

        setFocusItem(listView);
    }

    bool BellSettingsRelaxationWindow::onInput(const InputEvent &inputEvent)
    {
        if (listView->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            exit();
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    void BellSettingsRelaxationWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellSettingsRelaxationWindow::exit()
    {
        presenter->saveData();
        application->switchWindow(
            window::bell_finished::defaultName,
            BellFinishedWindowData::Factory::create("circle_success_big", name::window::main_window));
    }
} /* namespace gui */
