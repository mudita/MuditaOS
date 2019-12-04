#include "InputSelector.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "messages/AppMessage.hpp"
#include "../AppSpecialInput.hpp"
#include "Style.hpp"
#include <InputMode.hpp>

using namespace gui;

UiCharSelector::UiCharSelector(app::Application *app) : AppWindow(app, app::char_select )
{
    AppWindow::buildInterface();
    box = new GridLayout(0, title->offset_h(), style::window_width, bottomBar->offset_h() - title->offset_h(), {90, 90});
    for (auto schar : gui::special_chars)
    {
        auto el = new gui::Label(box, 0, 0, 80, 80);
        style::window::decorate(el);
        el->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
        el->setText(std::string(1, schar));
        el->setFont(style::window::font::medium);
        el->activatedCallback = [=](Item &it) {
            auto name = dynamic_cast<app::AppSpecialInput*>(application)->requester;
            LOG_INFO("handled %s for %s", el->getText().c_str(), name.c_str());
            setFocusItem(nullptr);
            sapm::ApplicationManager::messageSwitchSpecialInput( application,
                    std::make_unique<gui::SwitchSpecialChar>(gui::SwitchSpecialChar::Type::Response,name,el->getText()));
            return true;
        };
    }
    box->setVisible(true);
    box->setNavigation(nullptr, nullptr);
    /// TODO do not resize on each addWidget .. just once :(
    ///      shouldn't be needed here
    ///      add widget -> put widget in it's position
    box->resizeItems();
    addWidget(box);
    setFocusItem(box->getNavigationItem());
}

void UiCharSelector::onBeforeShow(ShowMode mode, SwitchData *data)
{
    auto ret = dynamic_cast<SwitchSpecialChar*>(data);
    if(ret) {
        LOG_INFO("handle for: %s", ret->requester.c_str());
        setFocusItem(box->getNavigationItem());
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        dynamic_cast<app::AppSpecialInput*>(application)->requester = ret->requester;
    }
}
void UiCharSelector::rebuild() {}
void UiCharSelector::buildInterface() {}
void UiCharSelector::destroyInterface()
{
    setFocusItem(nullptr);
}
