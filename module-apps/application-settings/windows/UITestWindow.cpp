#include "UITestWindow.hpp"
#include "../ApplicationSettings.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "i18/i18.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <Style.hpp>
#include <functional>
#include <memory>

namespace gui
{

UiTestWindow::UiTestWindow(app::Application *app) : AppWindow(app, "TEST_UI")
{
    AppWindow::buildInterface();
    buildInterface();
    // prebuild
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_select"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));
    setTitle("UI TEST");
    text = new gui::Text(this, style::window::default_left_margin, title->offset_h(), style::window_width - 2 * style::window::default_left_margin, 100,
                         "LoL ...");
    text->setEditMode(gui::Text::EditMode::EDIT);
    text->setFont(style::window::font::bigbold);
    // this sets application global keyboard - this is bad
    // thou this is fast to implement and possible right now out of box :|
    // this will crap itself after somebody changes keyboard in another text input :lol:
    text->setInputMode(new InputMode([=](const std::string val){application->setKeyboardProfile(val);}, InputMode::ABC, {InputMode::ABC, InputMode::abc}));
    ///// nooooo - me don't want to do dat on each call to :ext
    //#application->setKeyboardProfile(utils::localize.get("common_kbd_lower"));
    setFocusItem(text);
}

void UiTestWindow::rebuild() { LOG_INFO("Only if I have to!"); }

void UiTestWindow::buildInterface() {}

void UiTestWindow::destroyInterface()
{
    this->focusItem = nullptr;
    AppWindow::destroyInterface();
}

UiTestWindow::~UiTestWindow() { destroyInterface(); }

void UiTestWindow::onBeforeShow(ShowMode mode, SwitchData *data) {}

} // namespace gui
