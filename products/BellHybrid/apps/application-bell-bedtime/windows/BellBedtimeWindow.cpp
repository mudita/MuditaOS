// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellBedtimeWindow.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/TimeSetFmtSpinner.hpp>

#include <data/BellBedtimeStyle.hpp>
#include <module-gui/gui/input/InputEvent.hpp>

#include <module-gui/gui/widgets/text/TextFixedSize.hpp>
#include <module-gui/gui/widgets/ThreeBox.hpp>
#include <module-gui/gui/widgets/Image.hpp>

#include <widgets/SideListView.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include "service-appmgr/Controller.hpp" // for Controller

namespace gui
{
    BellBedtimeWindow::BellBedtimeWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::bell_bedtime::BellBedtimeWindowPresenter> &&windowPresenter,
        std::string name)
        : AppWindow(app, std::move(name)), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void BellBedtimeWindow::buildInterface()
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

    bool BellBedtimeWindow::onInput(const InputEvent &inputEvent)
    {
        if (listView->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            exit(true);
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    void BellBedtimeWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellBedtimeWindow::exit(bool showSuccessWindow)
    {
        presenter->saveData();
        if (showSuccessWindow) {
            application->switchWindow(gui::window::bell_finished::defaultName,
                                      BellFinishedWindowData::Factory::create("circle_success_big", "", "", true));
        }
        else {
            application->returnToPreviousWindow();
        }
    }
} /* namespace gui */
