#include "InputSelector.hpp"
#include "../AppSpecialInput.hpp"
#include "Style.hpp"
#include "TextConstants.hpp"
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
    const inline auto newline_x    = 20;
    const inline auto newline_y    = 20;
}; // namespace style::design

UiCharSelector::UiCharSelector(app::Application *app) : AppWindow(app, app::char_select)
{
    AppWindow::buildInterface();
    box = new GridLayout(style::window::default_left_margin,
                         title->offset_h(),
                         style::window_width - style::window::default_left_margin - style::window::default_right_margin,
                         bottomBar->offset_h() - title->offset_h(),
                         {style::design::char_grid_w, style::design::char_grid_h});
    box->addWidget(buildNewline());
    for (auto schar : gui::special_chars) {
        auto el = new gui::Label(box, 0, 0, style::design::char_label_w, style::design::char_label_h);
        style::window::decorate(el);
        el->setAlignment(
            gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
        el->setText(std::string(1, schar));
        el->setFont(style::window::font::medium);
        decorateActionActivated(el, el->getText());
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

Item *UiCharSelector::buildNewline()
{
    auto rect = new Rect(nullptr, 0, 0, style::design::char_label_w, style::design::char_label_h);
    rect->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_TOP | gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
    rect->setPenWidth(0);
    decorateActionActivated(rect, std::string(&text::newline, 1));
    new gui::Image(rect,
                   style::design::newline_x,
                   style::design::newline_y,
                   style::design::char_label_w,
                   style::design::char_label_h,
                   "enter_icon_alpha_W_M");
    return rect;
}

void UiCharSelector::decorateActionActivated(Item *it, const std::string &str)
{
    assert(it);
    assert(application);
    auto app = dynamic_cast<app::AppSpecialInput *>(application);
    assert(app);

    it->activatedCallback = [=](Item &it) {
        setFocusItem(nullptr);
        LOG_INFO("handled special char for %s", name.c_str());
        sapm::ApplicationManager::messageSwitchSpecialInput(
            application,
            std::make_unique<gui::SwitchSpecialChar>(gui::SwitchSpecialChar::Type::Response, app->requester, str));
        return true;
    };
}
