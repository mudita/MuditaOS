// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StatisticsWindow.hpp"

#include "MeditationMainWindow.hpp"

#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/SideListView.hpp>

namespace app::meditation
{
    using namespace gui;

    InputEvent transformKeyToKnobEvent(const InputEvent &inputEvent)
    {
        InputEvent newEvent{inputEvent};

        if (inputEvent.is(KeyCode::KEY_UP)) {
            newEvent.setKeyCode(gui::KeyCode::KEY_ENTER);
        }

        if (inputEvent.is(KeyCode::KEY_DOWN)) {
            newEvent.setKeyCode(KeyCode::KEY_RF);
        }

        return newEvent;
    }

    bool filterInputEvents(const gui::InputEvent &inputEvent)
    {
        return inputEvent.isShortRelease(KeyCode::KEY_ENTER);
    }

    StatisticsWindow::StatisticsWindow(app::ApplicationCommon *app,
                                       std::unique_ptr<app::meditation::contract::StatisticsPresenter> presenter)
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

        sideListView = new gui::SideListView(
            this, 0U, 0U, this->getWidth(), this->getHeight(), presenter->getPagesProvider(), PageBarType::None);
        sideListView->setEdges(RectangleEdge::None);
        sideListView->setBoundaries(gui::Boundaries::Continuous);

        sideListView->rebuildList(listview::RebuildType::Full);

        setFocusItem(sideListView);
    }

    void StatisticsWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        setFocusItem(sideListView);
    }

    bool StatisticsWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (filterInputEvents(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            presenter->handleExit();
            return true;
        }

        if (sideListView->onInput(transformKeyToKnobEvent(inputEvent))) {
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

    void StatisticsWindow::onClose(CloseReason reason)
    {
        if (reason != CloseReason::Popup) {
            presenter->eraseProviderData();
        }
    }
} // namespace app::meditation
