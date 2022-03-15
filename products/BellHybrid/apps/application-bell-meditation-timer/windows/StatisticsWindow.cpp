// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StatisticsWindow.hpp"

#include "MeditationMainWindow.hpp"

#include <common/windows/BellFinishedWindow.hpp>
#include <common/data/StyleCommon.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/SideListView.hpp>
#include <apps-common/InternalModel.hpp>

namespace app::meditation
{
    using namespace gui;
    StatisticsWindow::StatisticsWindow(app::ApplicationCommon *app,
                                       std::unique_ptr<app::meditation::contract::Presenter> presenter)
        : AppWindow(app, name), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void StatisticsWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void StatisticsWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);
    }

    void StatisticsWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        setFocusItem(sideListView);
    }

    bool StatisticsWindow::onInput(const gui::InputEvent &inputEvent)
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

    void StatisticsWindow::onClose(CloseReason reason)
    {
        presenter->eraseProviderData();
    }
} // namespace app::meditation
