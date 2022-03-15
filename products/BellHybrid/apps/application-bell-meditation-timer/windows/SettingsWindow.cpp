// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsWindow.hpp"

#include <common/data/StyleCommon.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/SideListView.hpp>

namespace app::meditation
{
    using namespace gui;
    SettingsWindow::SettingsWindow(app::ApplicationCommon *app,
                                   std::unique_ptr<app::meditation::contract::Presenter> presenter)
        : AppWindow(app, name), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void SettingsWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void SettingsWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        sideListView = new gui::SideListView(
            this, 0U, 0U, this->getWidth(), this->getHeight(), presenter->getPagesProvider(), PageBarType::None);
        sideListView->setEdges(RectangleEdge::None);

        sideListView->rebuildList(listview::RebuildType::Full);

        setFocusItem(sideListView);
    }

    void SettingsWindow::onBeforeShow(gui::ShowMode, gui::SwitchData *)
    {
        presenter->loadData();
        setFocusItem(sideListView);
    }

    bool SettingsWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (sideListView->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            presenter->handleEnter();
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

    void SettingsWindow::onClose(CloseReason)
    {
        presenter->eraseProviderData();
    }
} // namespace app::meditation
