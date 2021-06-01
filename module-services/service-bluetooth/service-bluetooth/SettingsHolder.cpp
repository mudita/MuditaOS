// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsHolder.hpp"
namespace bluetooth
{
    std::map<Settings, std::string> SettingsHolder::settingString{
        {Settings::DeviceName, settings::Bluetooth::deviceName},
        {Settings::Visibility, settings::Bluetooth::deviceVisibility},
        {Settings::State, settings::Bluetooth::state},
        {Settings::BondedDevices, settings::Bluetooth::bondedDevices},
        {Settings::BtKeys, settings::Bluetooth::btKeys},
        {Settings::ConnectedDevice, settings::Bluetooth::connectedDevice},

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
        auto returnIfEmpty = [](const std::string &value) {
            if (value.empty()) {
                LOG_ERROR("We got empty value");
                return false;
            }
            return true;
        };

        settingsProvider->registerValueChange(
            settings::Bluetooth::deviceVisibility, [this, returnIfEmpty](std::string value) {
                if (returnIfEmpty(value))
                    return;
                setValue(Settings::Visibility, value);
                settingsProvider->unregisterValueChange(settings::Bluetooth::deviceVisibility);
            });
        settingsProvider->registerValueChange(
            settings::Bluetooth::deviceName, [this, returnIfEmpty](std::string value) {
                if (returnIfEmpty(value))
                    return;
                setValue(Settings::DeviceName, value);
                settingsProvider->unregisterValueChange(settings::Bluetooth::deviceName);
            });
        settingsProvider->registerValueChange(
            settings::Bluetooth::bondedDevices, [this, returnIfEmpty](std::string value) {
                if (returnIfEmpty(value))
                    return;
                setValue(Settings::BondedDevices, value);
                settingsProvider->unregisterValueChange(settings::Bluetooth::bondedDevices);
            });
        settingsProvider->registerValueChange(settings::Bluetooth::btKeys, [this, returnIfEmpty](std::string value) {
            if (returnIfEmpty(value))
                return;
            setValue(Settings::BtKeys, value);
            settingsProvider->unregisterValueChange(settings::Bluetooth::btKeys);
        });
        settingsProvider->registerValueChange(settings::Bluetooth::state, [this, returnIfEmpty](std::string value) {
            if (returnIfEmpty(value))
                return;
            setValue(Settings::State, value);
            settingsProvider->unregisterValueChange(settings::Bluetooth::state);
            if (onStateChange) {
                onStateChange();
            }
        });
    }

    void SettingsHolder::deinit()
    {
        settingsProvider->deinit();
    }
} // namespace bluetooth
