#include "UITestWindow.hpp"
#include "../ApplicationSettings.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "i18/i18.hpp"
#include "messages/AppMessage.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <GridLayout.hpp>
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
        text = new gui::Text(this, style::window::default_left_margin, title->offset_h(), style::window_width - 2 * style::window::default_left_margin, 300,
                             "Add text, long press * adds special char, long press # changes input (and shows it in bottom bar) arrows walk, 1 in text mode "
                             "adds special chars too");
        text->setEditMode(gui::Text::EditMode::EDIT);
        text->setFont(style::window::font::medium);
        text->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &text) {
                bottomBar->setText(BottomBar::Side::CENTER, text);
                application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                bottomBar->getText(BottomBar::Side::CENTER);
            },
            [=]() {
                sapm::ApplicationManager::messageSwitchSpecialInput(
                    application, std::make_unique<gui::SwitchSpecialChar>(gui::SwitchSpecialChar::Type::Request, application->GetName()));
            }));
        text->setPenFocusWidth(0);
        setFocusItem(text);
    }

    void UiTestWindow::rebuild()
    {
        LOG_INFO("Only if I have to!");
    }

    void UiTestWindow::buildInterface()
    {
    }

    void UiTestWindow::destroyInterface()
    {
        this->focusItem = nullptr;
        AppWindow::destroyInterface();
    }

    UiTestWindow::~UiTestWindow()
    {
        destroyInterface();
    }

    void UiTestWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
    }

} // namespace gui
