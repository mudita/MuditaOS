#include <functional>
#include <memory>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationSettings.hpp"

#include "i18/i18.hpp"

#include "BtWindow.hpp"
#include "Label.hpp"
#include "Margins.hpp"

#include <Style.hpp>

#include <service-bluetooth/messages/BluetoothMessage.hpp>

namespace gui
{

BtWindow::BtWindow(app::Application *app) : AppWindow(app, "Bluetooth") { buildInterface(); }

void BtWindow::rebuild()
{
    destroyInterface();
    buildInterface();
}

void add_box_label(BoxLayout* layout, std::list<Item*> &it, UTF8 name, std::function<bool(Item&)> foo) {
    auto el = new gui::Label(layout, 0, 0, style::window_width, style::window::label::default_h);
    style::window::decorateOption(el);
    el->setText(name);
    el->activatedCallback = foo;
    it.push_back(el);
}

sys::ReturnCodes message_bt(app::Application* app, BluetoothMessage::Request req) {
    std::shared_ptr<BluetoothMessage> msg = std::make_shared<BluetoothMessage>(MessageType::BluetoothRequest, req);
    auto ret = sys::Bus::SendUnicast(msg,"ServiceBluetooth",app,5000);
    if(ret.first != sys::ReturnCodes::Success) {
        LOG_ERROR("err: %d",ret.first);
    }
    return ret.first;
}

void BtWindow::set_navigation() {
    if(!box) return;
    if (box->children.size() > 1) {
        auto nav_set = [=](auto el, auto end, auto dir) {
            for (; el != std::prev(end); ++el) {
                if ((*el)->visible == false) {  /// ignore if not visible
                    continue;
                }
                for (auto next = std::next(el); next != end; ++next) {    /// get next visible item
                    if ((*next)->visible) {
                        (*el)->setNavigationItem(dir, *next);  /// set navigation
                        break;
                    }
                }
            }
        };

        nav_set(box->children.begin(), box->children.end(), NavigationDirection::DOWN);
        nav_set(box->children.rbegin(), box->children.rend(), NavigationDirection::UP);
    }
}

void BtWindow::buildInterface()
{
    AppWindow::buildInterface();
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_select"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));
    topBar->setActive(TopBar::Elements::SIGNAL, true);
    topBar->setActive(TopBar::Elements::BATTERY, true);

    setTitle(utils::localize.get("app_settings_bt"));

    LOG_INFO("Create box layout");
    box = new gui::VBox(this, 0, title->offset_h(), style::window_width, 5*style::window::label::default_h);
    box->setPenWidth(style::window::label::border_no_focus_w);

    // TODO WIP: it's just for usability now
    // TODO scan should return async - handle that... (if in scan -> add to list and refresh if new on window)
    add_box_label(box, box_items, "Bluetooth on off", [=](Item&){
        LOG_DEBUG("Callback Bluetooth on");
        message_bt(application, BluetoothMessage::Request::Start);
        for (auto &el : box->children) {
            el->visible = true;
        }
        set_navigation();
        application->render(gui::RefreshModes::GUI_REFRESH_FAST);
        return true;
        });

    add_box_label(box, box_items, "  -> All devices", [=](Item&){
        LOG_DEBUG("Callback all devices");
        message_bt(application, BluetoothMessage::Request::Scan);
        return true;
    });

    add_box_label(box, box_items, "  -> PAN", [=](Item&){
        LOG_DEBUG("Callback start PAN");
        message_bt(application, BluetoothMessage::Request::PAN);
        return true;
    });

    add_box_label(box, box_items, "  -> Visibility", [=](Item&){
        LOG_DEBUG("Callback visibility");
        message_bt(application, BluetoothMessage::Request::Visible);
        return true;
    });

    // hide all elements except button for `bluetooth on off`
    std::for_each(std::next(box_items.begin()), box_items.end(), [](auto &el) { el->visible = false; } );

    box->resizeItems();

    set_navigation();

    this->focusItem = box_items.front();
    this->focusItem->setFocus(true);
}

void BtWindow::destroyInterface()
{
    this->focusItem = nullptr;
    AppWindow::destroyInterface();
}

BtWindow::~BtWindow() {
    destroyInterface();
}

void BtWindow::onBeforeShow(ShowMode mode, SwitchData *data) {}

} // namespace gui
