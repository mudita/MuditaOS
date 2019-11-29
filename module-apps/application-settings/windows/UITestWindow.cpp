#include "UITestWindow.hpp"
#include "../ApplicationSettings.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "i18/i18.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <Style.hpp>
#include <functional>
#include <memory>
#include <GridLayout.hpp>
#include "messages/AppMessage.hpp"

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
    text->setInputMode(new InputMode({InputMode::ABC, InputMode::abc}, [=](const UTF8 &text) {
        bottomBar->setText(BottomBar::Side::CENTER, text);
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        bottomBar->getText(BottomBar::Side::CENTER);
    }));
    text->setVisible(false);
    auto box = new GridLayout(0, title->offset_h(), style::window_width, bottomBar->offset_h() - title->offset_h(), {90,90} );
    for(auto schar : gui::special_chars) {
        auto el = new gui::Label(box, 0, 0, 80, 80);
        style::window::decorate(el);
        el->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
        el->setText(std::string(1,schar));
        el->setFont(style::window::font::medium);
        el->activatedCallback = [=](Item& it) {
            LOG_INFO("handled %s", el->getText().c_str()); return true;
        };
        // on click -> send onInput event with mapped & special keypress (...)
        // better - read special_keys_keymap and handle accordingly ... to be done
    }
    ));
    // TODO TODO attach(cb - show special characters, && input somehow)
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
