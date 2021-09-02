// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationListViewWindows.hpp"
#include "MeditationModel.hpp"
#include "Names.hpp"
#include "Style.hpp"

#include <gui/widgets/Label.hpp>
#include <gui/widgets/ListView.hpp>
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
    list = new gui::ListView(this,
                             listViewWindow::X,
                             listViewWindow::Y,
                             listViewWindow::Width,
                             listViewWindow::Height,
                             model,
                             gui::listview::ScrollBarType::Fixed);
    setFocusItem(list);
    bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
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
    list->rebuildList(gui::listview::RebuildType::InPlace);
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
    setTitle(utils::translate("common_options"));
    bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::Switch));
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
    setTitle(utils::translate("app_meditation_preparation_time"));
    bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::select));
}
