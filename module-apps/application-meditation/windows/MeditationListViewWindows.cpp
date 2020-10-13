#include "MeditationListViewWindows.hpp"
#include "application-meditation/widgets/MeditationListItemProviders.hpp"
#include "ListView.hpp"
#include "Label.hpp"
#include "Names.hpp"
#include <i18/i18.hpp>

using namespace gui;
namespace mainWindow = style::meditation::window::main;

MeditationListViewWindow::MeditationListViewWindow(app::Application *app, std::string windowName)
    : AppWindow(app, std::move(windowName))
{}

void MeditationListViewWindow::buildInterface()
{
    AppWindow::buildInterface();
    list = new gui::ListView(this, mainWindow::x, mainWindow::y, mainWindow::w, mainWindow::h, provider);
    setFocusItem(list);
}

void MeditationListViewWindow::destroyInterface()
{
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
    list->rebuildList();
    list->setVisible(true);
}

MeditationOptionsWindow::MeditationOptionsWindow(app::Application *app)
    : MeditationListViewWindow(app, app::window::name::meditation_options)
{
    provider = std::make_shared<MeditationOptionsProvider>(app);
    buildInterface();
}

void MeditationOptionsWindow::buildInterface()
{
    MeditationListViewWindow::buildInterface();
    setTitle(utils::localize.get("common_options"));
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
    topBar->setActive(TopBar::Elements::TIME, true);
}

PreparationTimeWindow::PreparationTimeWindow(app::Application *app)
    : MeditationListViewWindow(app, app::window::name::meditation_preparation)
{
    provider = std::make_shared<PrepTimeProvider>(app);
    buildInterface();
}

void PreparationTimeWindow::buildInterface()
{
    MeditationListViewWindow::buildInterface();
    setTitle(utils::localize.get("PreparationTime"));
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
    topBar->setActive(TopBar::Elements::TIME, true);
}
