// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationListViewWindows.hpp"

#include "application-meditation/widgets/MeditationModel.hpp"
#include "application-meditation/data/Style.hpp"
#include "Names.hpp"

#include "ListView.hpp"
#include "Label.hpp"
#include <i18n/i18n.hpp>

using namespace gui;
namespace listViewWindow = style::meditation::listView::window;

MeditationListViewWindow::MeditationListViewWindow(app::Application *app, std::string windowName)
    : AppWindow(app, std::move(windowName))
{}

void MeditationListViewWindow::buildInterface()
{
    AppWindow::buildInterface();
    model->createData();
    list = new gui::ListView(
        this, listViewWindow::X, listViewWindow::Y, listViewWindow::Width, listViewWindow::Height, model);
    setFocusItem(list);
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
}

void MeditationListViewWindow::destroyInterface()
{
    model->clearData();
    erase();
    invalidate();
}

void MeditationListViewWindow::rebuild()
{
    destroyInterface();
    buildInterface();
}

void MeditationListViewWindow::invalidate() noexcept
{
    list = nullptr;
}

void MeditationListViewWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    list->rebuildList(style::listview::RebuildType::InPlace);
    list->setVisible(true);
}

MeditationOptionsWindow::MeditationOptionsWindow(app::Application *app)
    : MeditationListViewWindow(app, app::window::name::meditation_options)
{
    model = std::make_shared<MeditationOptionsModel>(app);
    buildInterface();
}

void MeditationOptionsWindow::buildInterface()
{
    MeditationListViewWindow::buildInterface();
    setTitle(utils::localize.get("common_options"));
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::Switch));
}

PreparationTimeWindow::PreparationTimeWindow(app::Application *app)
    : MeditationListViewWindow(app, app::window::name::meditation_preparation)
{
    model = std::make_shared<PreparationTimeModel>(app);
    buildInterface();
}

void PreparationTimeWindow::buildInterface()
{
    MeditationListViewWindow::buildInterface();
    setTitle(utils::localize.get("app_meditation_preparation_time"));
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
}
