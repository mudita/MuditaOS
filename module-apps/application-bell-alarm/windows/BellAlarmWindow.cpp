// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellAlarmWindow.hpp"

#include <module-gui/gui/input/InputEvent.hpp>

namespace gui
{
    BellAlarmWindow::BellAlarmWindow(
        app::Application *app,
        std::unique_ptr<app::bell_alarm::BellAlarmWindowContract::Presenter> &&windowPresenter,
        std::string name) : AppWindow(app, std::move(name)), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void BellAlarmWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(true);
        bottomBar->setVisible(false);

        presenter->createData();

        sidelistview =
            new SideListView(this, 0U, 0U, this->getWidth(), this->getHeight(), presenter->getPagesProvider());
        sidelistview->setEdges(RectangleEdge::None);

        sidelistview->rebuildList(listview::RebuildType::Full);

        presenter->loadData();

        setFocusItem(sidelistview);
    }

    bool BellAlarmWindow::onInput(const InputEvent &inputEvent)
    {
        if (sidelistview->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            presenter->saveData();
            application->returnToPreviousWindow();
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    void BellAlarmWindow::rebuild()
    {
        erase();
        buildInterface();
    }
} /* namespace gui */
