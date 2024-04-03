// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellRefreshClockWindow.hpp"
#include "BellClockWindow.hpp"

#include <common/data/StyleCommon.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/SideListView.hpp>

namespace app::clock
{
    using namespace gui;
    BellRefreshClockWindow::BellRefreshClockWindow(app::ApplicationCommon *app,
                                                   std::unique_ptr<app::clock::contract::Presenter> presenter)
        : AppWindow(app, name), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void BellRefreshClockWindow::rebuild()
    {
        erase();
        buildInterface();
        isSaveNeeded = false;
    }

    void BellRefreshClockWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        sideListView = new gui::SideListView(
            this, 0U, 0U, this->getWidth(), this->getHeight(), presenter->getPagesProvider(), PageBarType::None);
        sideListView->setEdges(RectangleEdge::None);

        sideListView->rebuildList(listview::RebuildType::Full);

        presenter->loadData();
    }

    void BellRefreshClockWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        AppWindow::onBeforeShow(mode, data);
        setFocusItem(sideListView);
    }

    bool BellRefreshClockWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (sideListView->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            isSaveNeeded = true;
            switchToWindow();
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

    void BellRefreshClockWindow::switchToWindow()
    {
        application->switchWindow(gui::BellClockWindow::name);
    }

    void BellRefreshClockWindow::onClose(const CloseReason reason)
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
} // namespace app::clock
