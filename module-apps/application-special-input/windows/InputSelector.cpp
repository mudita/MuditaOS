#include "InputSelector.hpp"
#include "../AppSpecialInput.hpp"
#include "Style.hpp"
#include "messages/AppMessage.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <InputMode.hpp>

using namespace gui;

namespace style::design
{
    const inline auto char_grid_w  = 80;
    const inline auto char_grid_h  = 80;
    const inline auto char_label_w = 60;
    const inline auto char_label_h = 60;
}; // namespace style::design

UiCharSelector::UiCharSelector(app::Application *app) : AppWindow(app, app::char_select)
{
    AppWindow::buildInterface();
    box = new GridLayout(style::window::default_left_margin,
                         title->offset_h(),
                         style::window_width - style::window::default_left_margin - style::window::default_right_margin,
                         bottomBar->offset_h() - title->offset_h(),
                         {style::design::char_grid_w, style::design::char_grid_h});
    for (auto schar : gui::special_chars) {
        auto el = new gui::Label(box, 0, 0, style::design::char_label_w, style::design::char_label_h);
        style::window::decorate(el);
        el->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
        el->setText(std::string(1, schar));
        el->setFont(style::window::font::medium);
        el->activatedCallback = [=](Item &it) {
            auto name = dynamic_cast<app::AppSpecialInput *>(application)->requester;
            LOG_INFO("handled %s for %s", el->getText().c_str(), name.c_str());
            setFocusItem(nullptr);
            sapm::ApplicationManager::messageSwitchSpecialInput(
                application,
                std::make_unique<gui::SwitchSpecialChar>(gui::SwitchSpecialChar::Type::Response, name, el->getText()));
            return true;
        };
    }
    addWidget(box);
    setFocusItem(box);
}

void UiCharSelector::onBeforeShow(ShowMode mode, SwitchData *data)
{
    auto ret = dynamic_cast<SwitchSpecialChar *>(data);
    if (ret) {
        LOG_INFO("handle for: %s", ret->requester.c_str());
        setFocusItem(box);
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        dynamic_cast<app::AppSpecialInput *>(application)->requester = ret->requester;
    }
}
void UiCharSelector::rebuild()
{}
void UiCharSelector::buildInterface()
{}
void UiCharSelector::destroyInterface()
{
    setFocusItem(nullptr);
}
