// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsHolder.hpp"
namespace Bluetooth
{
    std::map<Settings, std::string> SettingsHolder::settingString{
        {Settings::DeviceName, settings::Bluetooth::deviceName},
        {Settings::Visibility, settings::Bluetooth::deviceVisibility},
        {Settings::State, settings::Bluetooth::state},
        {Settings::BondedDevices, settings::Bluetooth::bondedDevices},
        {Settings::BtKeys, settings::Bluetooth::btKeys},
    };

    auto SettingsHolder::getValue(const Settings setting) -> SettingEntry
    {
        return settingsMap[setting];
    }
    void SettingsHolder::setValue(const Settings &newSetting, const SettingEntry &value)
    {
        settingsMap[newSetting] = value;

        settingsProvider->setValue(settingString[newSetting], std::visit(StringVisitor(), value));
        LOG_INFO("setting %s set: %s", settingString[newSetting].c_str(), std::visit(StringVisitor(), value).c_str());
    }
    SettingsHolder::SettingsHolder(std::unique_ptr<settings::Settings> settingsPtr)
        : settingsProvider(std::move(settingsPtr))
    {
        settingsProvider->registerValueChange(settings::Bluetooth::deviceVisibility, [this](std::string value) {
            setValue(Settings::Visibility, value);
            settingsProvider->unregisterValueChange(settings::Bluetooth::deviceVisibility);
        });
        settingsProvider->registerValueChange(settings::Bluetooth::deviceName, [this](std::string value) {
            setValue(Settings::DeviceName, value);
            settingsProvider->unregisterValueChange(settings::Bluetooth::deviceName);
        });
        settingsProvider->registerValueChange(settings::Bluetooth::bondedDevices, [this](std::string value) {
            setValue(Settings::BondedDevices, value);
            settingsProvider->unregisterValueChange(settings::Bluetooth::bondedDevices);
        });
        settingsProvider->registerValueChange(settings::Bluetooth::btKeys, [this](std::string value) {
            setValue(Settings::BtKeys, value);
            settingsProvider->unregisterValueChange(settings::Bluetooth::btKeys);
        });
        settingsProvider->registerValueChange(settings::Bluetooth::state, [this](std::string value) {
            setValue(Settings::State, value);
            settingsProvider->unregisterValueChange(settings::Bluetooth::state);
            if (onStateChange) {
                onStateChange();
            }
        });
    }
} // namespace Bluetooth
