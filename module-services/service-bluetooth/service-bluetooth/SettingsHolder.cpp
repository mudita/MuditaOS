// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsHolder.hpp"
#include <log/log.hpp>
namespace Bluetooth
{
    std::map<Settings, std::string> SettingsHolder::settingString{
        {Settings::DeviceName, settings::Bluetooth::deviceName},
        {Settings::Visibility, settings::Bluetooth::deviceVisibility},
        {Settings::Power, settings::Bluetooth::state},
        {Settings::BondedDevices, settings::Bluetooth::bondedDevices},
    };

    auto SettingsHolder::getValue(const Settings setting) -> SettingEntry
    {
        return settingsMap[setting];
    }
    void SettingsHolder::setValue(const Settings &newSetting, const SettingEntry &value)
    {
        settingsMap[newSetting] = value;

        settingsProvider->setValue(settingString[newSetting], std::visit(StringVisitor(), value));
    }
    SettingsHolder::SettingsHolder(std::unique_ptr<settings::Settings> settingsPtr)
        : settingsProvider(std::move(settingsPtr))
    {
        settingsProvider->registerValueChange(settings::Bluetooth::state, [this](std::string value) {
            setValue(Settings::Power, value);
            this->settingsProvider->unregisterValueChange(settings::Bluetooth::state);
        });
        settingsProvider->registerValueChange(settings::Bluetooth::deviceVisibility, [this](std::string value) {
            setValue(Settings::Visibility, value);
            this->settingsProvider->unregisterValueChange(settings::Bluetooth::deviceVisibility);
        });
        settingsProvider->registerValueChange(settings::Bluetooth::deviceName, [this](std::string value) {
            setValue(Settings::DeviceName, value);
            this->settingsProvider->unregisterValueChange(settings::Bluetooth::deviceName);
        });
        settingsProvider->registerValueChange(settings::Bluetooth::bondedDevices, [this](std::string value) {
            setValue(Settings::BondedDevices, value);
            this->settingsProvider->unregisterValueChange(settings::Bluetooth::bondedDevices);
        });
    }
} // namespace Bluetooth
