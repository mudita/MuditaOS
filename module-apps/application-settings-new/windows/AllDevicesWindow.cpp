// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AllDevicesWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "application-settings-new/widgets/SettingsStyle.hpp"

#include "OptionSetting.hpp"
#include "DialogMetadata.hpp"
#include "DialogMetadataMessage.hpp"
#include <Constants.hpp>

#include <InputEvent.hpp>
#include <i18n/i18n.hpp>
#include <service-bluetooth/BluetoothMessage.hpp>
#include <service-bluetooth/messages/BondedDevices.hpp>
#include "application-settings-new/data/BondedDevicesData.hpp"
namespace gui
{

    AllDevicesWindow::AllDevicesWindow(app::Application *app) : OptionWindow(app, gui::window::name::all_devices)
    {
        setTitle(utils::localize.get("app_settings_bluetooth_all_devices"));
        sys::Bus::SendUnicast(
            std::make_shared<::message::bluetooth::RequestBondedDevices>(), service::name::bluetooth, application);
    }

    void AllDevicesWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        clearOptions();
        if (const auto newData = dynamic_cast<BondedDevicesData *>(data); newData != nullptr) {
            addOptions(allDevicesOptionsList(newData->getDevices()));
        }
    }

    auto AllDevicesWindow::onInput(const InputEvent &inputEvent) -> bool
    {

        if (inputEvent.state == InputEvent::State::keyReleasedShort) {
            if (inputEvent.keyCode == KeyCode::KEY_LEFT) {
                sys::Bus::SendUnicast(std::make_shared<BluetoothMessage>(BluetoothMessage::Request::Scan),
                                      "ServiceBluetooth",
                                      application);
                gui::DialogMetadata meta;
                meta.icon                        = "search_big";
                meta.text                        = utils::localize.get("app_settings_bluetooth_searching_devices");
                meta.title                       = utils::localize.get("app_settings_bluetooth_add_device");
                auto data                        = std::make_unique<gui::DialogMetadataMessage>(meta);
                data->ignoreCurrentWindowOnStack = true;
                application->switchWindow(gui::window::name::dialog_settings, std::move(data));
                return true;
            }
        }

        return AppWindow::onInput(inputEvent);
    }

    auto AllDevicesWindow::allDevicesOptionsList(const std::vector<Devicei> &devicesList) -> std::list<Option>
    {
        std::list<gui::Option> optionsList;

        for (const auto &device : devicesList) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                device.name,
                [=](gui::Item &item) {
                    LOG_DEBUG("Device: %s", device.name.c_str());
                    return true;
                },
                nullptr,
                nullptr,
                gui::option::SettingRightItem::Bt));
        }

        leftArrowImage = new gui::Image(this,
                                        style::settings::window::leftArrowImage::x,
                                        style::settings::window::leftArrowImage::y,
                                        style::settings::window::leftArrowImage::w,
                                        style::settings::window::leftArrowImage::h,
                                        "arrow_left");
        crossImage     = new gui::Image(this,
                                    style::settings::window::crossImage::x,
                                    style::settings::window::crossImage::y,
                                    style::settings::window::crossImage::w,
                                    style::settings::window::crossImage::h,
                                    "cross");

        return optionsList;
    }
} // namespace gui
