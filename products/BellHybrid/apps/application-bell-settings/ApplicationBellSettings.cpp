// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellSettings.hpp"
#include "TimeUnitsPresenter.hpp"
#include "models/TemperatureUnitModel.hpp"
#include "windows/BellSettingsAdvancedWindow.hpp"
#include "windows/BellSettingsFinishedWindow.hpp"
#include "windows/BellSettingsTimeUnitsWindow.hpp"
#include "windows/BellSettingsWindow.hpp"

#include <apps-common/windows/Dialog.hpp>

namespace app
{
    ApplicationBellSettings::ApplicationBellSettings(std::string name,
                                                     std::string parent,
                                                     sys::phone_modes::PhoneMode mode,
                                                     sys::bluetooth::BluetoothMode bluetoothMode,
                                                     StartInBackground startInBackground)
        : Application(std::move(name), std::move(parent), mode, bluetoothMode, startInBackground)
    {}

    sys::ReturnCodes ApplicationBellSettings::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }
        createUserInterface();
        return sys::ReturnCodes::Success;
    }

    void ApplicationBellSettings::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string &name) {
            return std::make_unique<gui::BellSettingsWindow>(app);
        });

        windowsFactory.attach(gui::window::name::bellSettingsAdvanced, [](Application *app, const std::string &name) {
            return std::make_unique<gui::BellSettingsAdvancedWindow>(app);
        });

        windowsFactory.attach(gui::window::name::bellSettingsTimeUnits, [](Application *app, const std::string &name) {
            auto temperatureUnitModel = std::make_unique<bell_settings::TemperatureUnitModel>(app);
            auto timeUnitsProvider    = std::make_shared<bell_settings::TimeUnitsModel>(app);
            auto presenter            = std::make_unique<bell_settings::TimeUnitsWindowPresenter>(timeUnitsProvider,
                                                                                       std::move(temperatureUnitModel));
            return std::make_unique<gui::BellSettingsTimeUnitsWindow>(app, std::move(presenter));
        });

        windowsFactory.attach(gui::window::name::bellSettingsFinished, [](Application *app, const std::string &name) {
            return std::make_unique<gui::BellSettingsFinishedWindow>(app);
        });
    }

    sys::MessagePointer ApplicationBellSettings::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }
        return std::make_shared<sys::ResponseMessage>();
    }
} // namespace app
