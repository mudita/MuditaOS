// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <functional>
#include <memory>

#include <service-appmgr/model/ApplicationManager.hpp>

#include "../ApplicationSettings.hpp"

#include <i18n/i18n.hpp>

#include "BtScanWindow.hpp"
#include "Label.hpp"
#include "Margins.hpp"

#include <Style.hpp>

extern "C"
{
#include <module-bluetooth/lib/btstack/src/btstack_util.h>
}

namespace gui
{

    BtScanWindow::BtScanWindow(app::Application *app, std::vector<Devicei> devices)
        : AppWindow(app, name::window::name_btscan), devices(devices)
    {
        buildInterface();
    }

    void BtScanWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void add_box_label2(VBox *layout, UTF8 name, std::function<bool(Item &)> foo)
    {
        auto el = new gui::Label(nullptr, 0, 0, layout->getWidth(), style::window::label::default_h);
        style::window::decorateOption(el);
        el->setText(name);
        el->activatedCallback = std::move(foo);
        layout->addWidget(el);
    }

    sys::ReturnCodes message_bt2(app::Application *app, BluetoothMessage::Request req)
    {
        std::shared_ptr<BluetoothMessage> msg = std::make_shared<BluetoothMessage>(req);
        auto ret                              = app->bus.sendUnicast(msg, "ServiceBluetooth");
        if (!ret) {
            LOG_ERROR("err: %d", static_cast<int>(ret));
        }
        return ret ? sys::ReturnCodes::Success : sys::ReturnCodes::Failure;
    }

    void BtScanWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        setTitle(utils::translate("BT_scan_results"));

        box = new gui::VBox(
            this, 0, style::window::default_vertical_pos, style::window_width, 7 * style::window::label::default_h);

        for (auto device : devices) {
            add_box_label2(box, device.name, [=](Item &) {
                LOG_DEBUG("Device added to list");

                std::shared_ptr<BluetoothAddrMessage> msg =
                    std::make_shared<BluetoothAddrMessage>(bd_addr_to_str(device.address));
                application->bus.sendUnicastSync(msg, "ServiceBluetooth", 5000);

                message_bt2(application, BluetoothMessage::Request::StopScan);
                // message_bt2(application, BluetoothMessage::Request::Start);

                return true;
            });
        }
        for (auto &el : box->children) {
            el->visible = true;
        }
        box->setPenWidth(style::window::default_border_no_focus_w);
        box->setVisible(true);
        setFocusItem(box);
        application->render(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    bool BtScanWindow::onInput(const InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }

    void BtScanWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    void BtScanWindow::invalidate() noexcept
    {
        box = nullptr;
    }

    void BtScanWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        auto newData = static_cast<DeviceData *>(data);
        devices      = newData->getDevices();
        rebuild();
    }

} // namespace gui
