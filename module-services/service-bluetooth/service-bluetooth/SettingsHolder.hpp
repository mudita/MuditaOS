// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once
#include <json11.hpp>
#include <string>
#include <variant>
#include <service-db/Settings.hpp>
#include <Utils.hpp>
#include "SettingsSerializer.hpp"

namespace bluetooth
{

    enum class Settings
    {
        DeviceName,
        Visibility,
        State,
        BondedDevices,
        BtKeys,
        ConnectedDevice
    };

    constexpr inline auto trueStr  = "true";
    constexpr inline auto falseStr = "false";

    using SettingEntry = std::variant<int, bool, std::string>;
    struct StringVisitor
    {
        auto operator()(int input) const -> std::string
        {
            return utils::to_string(input);
        }
        auto operator()(bool input) const -> std::string
        {
            return input ? trueStr : falseStr;
        }
        auto operator()(const std::string &input) const -> std::string
        {
            return input;
        }
    };

    struct IntVisitor
    {
        auto operator()(const std::string &input) const -> int
        {
            int value;
            utils::toNumeric(input, value);
            return value;
        }
        auto operator()(bool input) const -> int
        {
            return input;
        }
        auto operator()(int input) const -> int
        {
            return input;
        }
    };

    struct BoolVisitor
    {
        auto operator()(const std::string &input) const -> bool
        {
            return input == trueStr;
        }
        auto operator()(bool input) const -> bool
        {
            return input;
        }
        auto operator()(int input) const -> bool
        {
            return input != 0;
        }
    };

    class SettingsHolder
    {
      public:
        SettingsHolder()          = default;
        virtual ~SettingsHolder() = default;
        explicit SettingsHolder(std::unique_ptr<settings::Settings> settingsPtr);
        virtual auto getValue(const Settings setting) -> SettingEntry;
        virtual void setValue(const Settings &newSetting, const SettingEntry &value);
        std::function<void()> onStateChange;
        std::function<void(const std::string &)> onLinkKeyAdded;
        void deinit();

      private:
        static std::map<Settings, std::string> settingString;
        std::unique_ptr<settings::Settings> settingsProvider;
        std::map<Settings, SettingEntry> settingsMap;
    };
} // namespace bluetooth
