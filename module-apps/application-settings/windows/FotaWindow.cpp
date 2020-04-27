#include "FotaWindow.hpp"

namespace gui {

FotaWindow::FotaWindow(app::Application *app)
    :AppWindow(app, window::fota_window )
{
    buildInterface();
}

FotaWindow::~FotaWindow()
{
    destroyInterface();
}

void FotaWindow::onBeforeShow(ShowMode /*mode*/, SwitchData */*data*/)
{

}

void FotaWindow::rebuild()
{
    destroyInterface();
    buildInterface();
}

void FotaWindow::buildInterface()
{
    LOG_INFO("Build Fota Window");
    AppWindow::buildInterface();
    setTitle("Modem Firmware update (FOTA)");

    topBar->setActive(TopBar::Elements::SIGNAL, true);
    topBar->setActive(TopBar::Elements::BATTERY, true);

    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_select"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

    mainBox = new gui::VBox(this, 0, title->offset_h(), style::window_width, style::window_height);
    mainBox->setPenWidth(style::window::default_border_no_focus_w);
}

void FotaWindow::destroyInterface()
{
    this->focusItem = nullptr;
    AppWindow::destroyInterface();

}

} // namespace gui
