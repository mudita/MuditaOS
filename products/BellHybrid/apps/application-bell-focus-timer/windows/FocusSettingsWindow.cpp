// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FocusSettingsWindow.hpp"

#include <common/data/StyleCommon.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/SideListView.hpp>

namespace app::focus
{
    using namespace gui;

    SettingsWindow::SettingsWindow(app::ApplicationCommon *app,
                                   std::unique_ptr<SettingsContract::Presenter> presenter,
                                   const std::string &name)
        : AppWindow(app, name), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void SettingsWindow::rebuild()
    {
        erase();
        buildInterface();
        isSaveNeeded = false;
    }

    void SettingsWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        sideListView = new SideListView(
            this, 0U, 0U, this->getWidth(), this->getHeight(), presenter->getPagesProvider(), PageBarType::None);
        sideListView->setEdges(RectangleEdge::None);

        sideListView->rebuildList(listview::RebuildType::Full);

        presenter->loadData();
    }

    void SettingsWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        AppWindow::onBeforeShow(mode, data);
        setFocusItem(sideListView);
    }

    bool SettingsWindow::onInput(const InputEvent &inputEvent)
    {
        if (sideListView->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            exit();
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            presenter->exitWithoutSave();
        }

        return AppWindow::onInput(inputEvent);
    }

    void SettingsWindow::switchToExitWindow()
    {
        application->switchWindow(gui::window::bell_finished::defaultName,
                                  BellFinishedWindowData::Factory::create("circle_success_big", window::name::main));
    }

    void SettingsWindow::onClose(const CloseReason reason)
    {
        if (reason != CloseReason::Popup) {
            if (isSaveNeeded) {
                presenter->exitWithSave();
            }
            else {
                presenter->exitWithoutSave();
            }
        }
    }

    void SettingsWindow::exit()
    {
        isSaveNeeded = true;
        switchToExitWindow();
    }
} // namespace app::focus
