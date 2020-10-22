// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AllDevicesWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "application-settings-new/widgets/SettingsStyle.hpp"

#include "OptionSetting.hpp"
#include "DialogMetadata.hpp"
#include "DialogMetadataMessage.hpp"

#include <InputEvent.hpp>
#include <i18/i18.hpp>
#include <service-bluetooth/messages/BluetoothMessage.hpp>

namespace gui
{

    AllDevicesWindow::AllDevicesWindow(app::Application *app) : OptionWindow(app, gui::window::name::all_devices)
    {
        setTitle(utils::localize.get("app_settings_bluetooth_all_devices"));
        addOptions(allDevicesOptionsList());
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

    auto AllDevicesWindow::allDevicesOptionsList() -> std::list<Option>
    {
        std::list<gui::Option> optionsList;

        std::vector<Devicei> devices{Devicei("Paired_device1"),
                                     Devicei("Paired_device2"),
                                     Devicei("Paired_device3"),
                                     Devicei("Paired_device4"),
                                     Devicei("Paired_device5")};

        for (const auto &device : devices) {
            optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
                device.name,
                [=](gui::Item &item) {
                    LOG_DEBUG("Device: %s", device.name.c_str());
                    return true;
                },
                nullptr,
                nullptr,
                RightIcon::Bt));
        }

        topBar->setActive(TopBar::Elements::SIGNAL, false);
        topBar->setActive(TopBar::Elements::BATTERY, false);
        topBar->setActive(TopBar::Elements::SIM, false);

        leftArrowImage = new gui::Image(this,
                                        style::settings::window::allDevices::leftArrowImage::x,
                                        style::settings::window::allDevices::leftArrowImage::y,
                                        style::settings::window::allDevices::leftArrowImage::w,
                                        style::settings::window::allDevices::leftArrowImage::h,
                                        "arrow_left");
        crossImage     = new gui::Image(this,
                                    style::settings::window::allDevices::crossImage::x,
                                    style::settings::window::allDevices::crossImage::y,
                                    style::settings::window::allDevices::crossImage::w,
                                    style::settings::window::allDevices::crossImage::h,
                                    "cross");

        return optionsList;
    }
} // namespace gui
