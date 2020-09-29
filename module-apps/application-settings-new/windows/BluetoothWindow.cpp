#include "BluetoothWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <i18/i18.hpp>

namespace gui
{

    BluetoothWindow::BluetoothWindow(app::Application *app) : OptionWindow(app, gui::window::name::bluetooth)
    {
        buildInterface();
    }

    void BluetoothWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        rebuildOptionList();
    }

    auto BluetoothWindow::bluetoothOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> options;

        options.emplace_back(gui::Option{utils::localize.get("app_settings_bluetooth_main"), [=](gui::Item &item) {
                                             switchHandler(isBluetoothSwitchOn);
                                             return true;
                                         }});
        if (isBluetoothSwitchOn) {
            options.emplace_back(gui::Option{utils::localize.get("app_settings_bluetooth_all_devices_indent"),
                                             [=](gui::Item &item) {
                                                 this->application->switchWindow(gui::window::name::all_devices,
                                                                                 gui::ShowMode::GUI_SHOW_INIT);
                                                 return true;
                                             },
                                             gui::Arrow::Enabled});
            options.emplace_back(
                gui::Option{utils::localize.get("app_settings_bluetooth_phone_visibility"), [=](gui::Item &item) {
                                switchHandler(isPhoneVisibilitySwitchOn);
                                return true;
                            }});
            if (isPhoneVisibilitySwitchOn) {
                options.emplace_back(gui::Option{utils::localize.get("app_settings_bluetooth_phone_name_indent"),
                                                 [=](gui::Item &item) {
                                                     this->application->switchWindow(gui::window::name::phone_name,
                                                                                     gui::ShowMode::GUI_SHOW_INIT);
                                                     return true;
                                                 },
                                                 gui::Arrow::Enabled});
            }
        }

        return options;
    }

    void BluetoothWindow::switchHandler(bool &switchState)
    {
        switchState = !switchState;
        rebuildOptionList();
    }

    void BluetoothWindow::rebuildOptionList()
    {
        clearOptions();
        addOptions(bluetoothOptionsList());
    }

} // namespace gui
