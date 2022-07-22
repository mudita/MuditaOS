// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StatisticsWindow.hpp"

#include "MeditationMainWindow.hpp"

#include <ListView.hpp>
#include <common/data/StyleCommon.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <module-gui/gui/input/InputEvent.hpp>
#include <apps-common/InternalModel.hpp>

namespace
{
    constexpr auto height     = 400;
    constexpr auto width      = 380;
    constexpr auto top_margin = 41;
} // namespace

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

        list = new ListView(this,
                            style::window::default_left_margin,
                            top_margin,
                            width,
                            height,
                            presenter->getPagesProvider(),
                            listview::ScrollBarType::Fixed);
        list->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        list->rebuildList();
    }

    void StatisticsWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        setFocusItem(list);
    }

    bool StatisticsWindow::onInput(const gui::InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }

    void StatisticsWindow::onClose(CloseReason reason)
    {
        presenter->eraseProviderData();
    }
} // namespace app::meditation
