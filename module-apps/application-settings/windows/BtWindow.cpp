// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <functional>
#include <memory>

#include <service-appmgr/model/ApplicationManager.hpp>

#include "../ApplicationSettings.hpp"
#include "../windows/BtScanWindow.hpp"

#include <i18n/i18n.hpp>

#include "BtWindow.hpp"
#include "Label.hpp"
#include "Margins.hpp"

#include <Style.hpp>

#include <service-bluetooth/BluetoothMessage.hpp>

namespace gui
{

    BtWindow::BtWindow(app::Application *app) : AppWindow(app, "Bluetooth")
    {
        buildInterface();
    }

    void BtWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void add_box_label(VBox *layout, UTF8 name, std::function<bool(Item &)> foo)
    {
        auto el = new gui::Label(nullptr, 0, 0, layout->getWidth(), style::window::label::default_h);
        style::window::decorateOption(el);
        el->setText(name);
        el->activatedCallback = foo;
        layout->addWidget(el);
    }

    sys::ReturnCodes message_bt(app::Application *app, BluetoothMessage::Request req)
    {
        std::shared_ptr<BluetoothMessage> msg = std::make_shared<BluetoothMessage>(req);
        auto ret                              = sys::Bus::SendUnicast(msg, "ServiceBluetooth", app);
        if (!ret) {
            LOG_ERROR("err: %d", static_cast<int>(ret));
        }
        return ret ? sys::ReturnCodes::Success : sys::ReturnCodes::Failure;
    }

    void BtWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        setTitle(utils::localize.get("app_settings_bt"));

        LOG_INFO("Create box layout");
        box = new gui::VBox(this, 0, title->offset_h(), style::window_width, 7 * style::window::label::default_h);
        box->setEdges(RectangleEdge::None);

        // TODO WIP: it's just for usability now
        // TODO scan should return async - handle that... (if in scan -> add to list and refresh if new on window)
        add_box_label(box, "Bluetooth on off", [=](Item &) {
            LOG_DEBUG("Callback Bluetooth on");
            message_bt(application, BluetoothMessage::Request::Start);
            for (auto &el : box->children) {
                el->visible = true;
            }
            box->setVisible(true);
            application->render(gui::RefreshModes::GUI_REFRESH_FAST);
            return true;
        });

        add_box_label(box, "  -> All devices", [=](Item &) {
            LOG_DEBUG("Callback all devices");
            message_bt(application, BluetoothMessage::Request::Scan);
            application->setActiveWindow(gui::name::window::name_btscan);
            return true;
        });

        add_box_label(box, "  -> PAN", [=](Item &) {
            LOG_DEBUG("Callback start PAN");
            message_bt(application, BluetoothMessage::Request::PAN);
            return true;
        });

        add_box_label(box, "  -> Set visible", [=](Item &) {
            LOG_DEBUG("Callback set visibility");
            message_bt(application, BluetoothMessage::Request::Visible);
            return true;
        });

        add_box_label(box, "  -> Play", [=](Item &) {
            LOG_DEBUG("Start playback");
            message_bt(application, BluetoothMessage::Request::Play);
            return true;
        });

        add_box_label(box, "  -> Stop", [=](Item &) {
            LOG_DEBUG("Stop playback");
            message_bt(application, BluetoothMessage::Request::StopPlayback);
            return true;
        });

        // hide all elements except button for `bluetooth on off` - this would cause infinite loop
        std::for_each(std::next(box->children.begin()), box->children.end(), [](auto &el) { el->visible = false; });
        setFocusItem(box);
    }

    bool BtWindow::onInput(const InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }

    void BtWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    void BtWindow::invalidate() noexcept
    {
        box = nullptr;
    }

    void BtWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

} // namespace gui
